package vn.nckh27pa.fallsafe.bluetooth

import org.junit.Assert.*
import org.junit.Test
import vn.nckh27pa.fallsafe.protocol.Esp32SensorStatus

class SensorFailureCompatibilityTest {
    private val unavailable = """{"protocolVersion":1,"deviceId":"FALLSAFE-AB12","sequenceNumber":1,"timestampMs":20,"accelXMs2":null,"accelYMs2":null,"accelZMs2":null,"gyroXDps":null,"gyroYDps":null,"gyroZDps":null,"pressurePa":100000.0,"temperatureC":25,"altitudeDeltaM":null,"batteryPercent":-1,"batteryVoltageMv":null,"isCharging":false,"sosButtonPressed":false,"sensorQuality":0}"""
    @Test fun unavailableIsStatusNotMotion() {
        assertNotNull(Esp32SensorStatus.decodeUnavailable(unavailable))
        assertNull(Esp32SensorStatus.decodeUnavailable(unavailable.replace("\"sensorQuality\":0", "\"sensorQuality\":80")))
        assertNull(Esp32SensorStatus.decodeUnavailable(unavailable.replace("\"accelXMs2\":null", "\"accelXMs2\":0")))
        assertNull(Esp32SensorStatus.decodeUnavailable(unavailable.dropLast(1)+",\"sensorQuality\":0}"))
    }
    @Test fun fullProfileFitsAndFramedAckReachesUi() {
        val payload = BleProfilePayload.buildJson(BleFallProfile.DEFAULT)
        assertTrue(payload.toByteArray().size <= 512 - 3)
        assertEquals(14, com.google.gson.JsonParser.parseString(payload).asJsonObject.size())
        val client = FallSafeBleClient()
        assertFalse(client.writeProfile(BleFallProfile.DEFAULT)) // disconnected is not success
        val ack = """{"protocolVersion":1,"commandId":"profile","deviceId":"FALLSAFE-AB12","timestampMs":20,"commandStatus":"COMPLETED","errorCode":null,"message":"Applied successfully"}"""
        for (frame in requireNotNull(Framing.encode(23, 5, 90, ack.toByteArray()))) {
            client.processNotification(BleGattUuids.PROFILE_WRITE_UUID, frame)
        }
        assertEquals(ack, client.latestAckJson.value)
    }
    @Test fun framedFailureClearsOldSampleAndRecovers() {
        val client = FallSafeBleClient()
        var samples = 0
        client.onSensorPacket = { samples++ }
        val healthy = unavailable.replace(":null", ":0").replace("\"sensorQuality\":0", "\"sensorQuality\":80")
        client.processNotification(BleGattUuids.TELEMETRY_NOTIFY_UUID, healthy.toByteArray())
        assertNotNull(client.latestSensorPacket.value)
        for (mtu in listOf(23, 512)) {
            for (frame in requireNotNull(Framing.encode(mtu, 1, mtu.toLong(), unavailable.toByteArray()))) {
                client.processNotification(BleGattUuids.TELEMETRY_NOTIFY_UUID, frame)
            }
            assertNull(client.latestSensorPacket.value)
            assertNotNull(client.latestSensorStatus.value)
            assertEquals(100000f, client.latestSensorStatus.value!!.pressurePa!!, 0.01f)
        }
        assertEquals(1, samples)
        client.processNotification(BleGattUuids.TELEMETRY_NOTIFY_UUID, healthy.toByteArray())
        assertNull(client.latestSensorStatus.value)
        assertEquals(2, samples)
        client.disconnect()
        assertNull(client.latestSensorPacket.value)
        assertNull(client.latestSensorStatus.value)
        assertNull(client.latestAckJson.value)
    }
}
