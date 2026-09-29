package vn.nckh27pa.fallsafe.bluetooth

import core.MonotonicClock
import org.junit.Assert.*
import org.junit.Test
import vn.nckh27pa.fallsafe.DemoController
import core.State

class BleTestSosTest {
    @Test fun parsesTestMarkerWithoutChangingRealEvents() {
        val test = BleEventPacket.parse("""{"eventId":"40001","eventType":"SOS_PRESSED","test":true}""")
        val real = BleEventPacket.parse("""{"eventId":"1","eventType":"SOS_PRESSED","test":false}""")
        assertTrue(test!!.test)
        assertFalse(real!!.test)
    }

    @Test fun realSosSupersedesActiveTestSos() {
        val controller = DemoController(clock = MonotonicClock { 0L })
        controller.testHelp()
        val testEventId = controller.snapshot.eventId
        assertTrue(controller.testSosActive)
        assertTrue(testEventId > 0)

        controller.help()
        assertFalse(controller.testSosActive)
        assertTrue(controller.snapshot.eventId > testEventId)

        controller.testHelp()
        assertFalse(controller.testSosActive)
    }

    @Test fun realSosSirenStopsOnMuteAndCompletionWithoutRestartingOnDuplicate() {
        val controller = DemoController(clock = MonotonicClock { 0L })
        val soundStates = mutableListOf<Boolean>()
        controller.onSosSoundChanged = soundStates::add

        controller.testHelp()
        assertEquals(false, soundStates.last())

        controller.help()
        assertEquals(State.AWAITING_HELP, controller.snapshot.state)
        assertEquals(true, soundStates.last())

        controller.muteSiren()
        assertEquals(false, soundStates.last())
        controller.help() // BLE may retransmit the same SOS event.
        assertTrue(controller.sirenMuted)
        assertEquals(false, soundStates.last())

        controller.complete()
        assertEquals(State.MONITORING, controller.snapshot.state)
        assertEquals(false, soundStates.last())
    }

    @Test fun bleSosWarnsBeforeDispatchAndCanBeCancelled() {
        var now = 0L
        val controller = DemoController(clock = MonotonicClock { now })
        val soundStates = mutableListOf<Boolean>()
        controller.onSosSoundChanged = soundStates::add

        controller.beginSosVerification()
        assertEquals(State.VERIFYING, controller.snapshot.state)
        assertEquals(10_000L, controller.snapshot.remainingMs)
        assertEquals(true, soundStates.last())

        now = 9_000L
        controller.safe()
        assertEquals(State.MONITORING, controller.snapshot.state)
        assertEquals(false, soundStates.last())

        controller.beginSosVerification()
        now = 19_000L
        controller.session.tick()
        controller.refresh()
        assertEquals(State.AWAITING_HELP, controller.snapshot.state)
        assertEquals(true, soundStates.last())
    }
}
