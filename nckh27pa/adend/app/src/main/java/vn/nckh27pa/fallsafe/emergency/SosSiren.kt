package vn.nckh27pa.fallsafe.emergency

import android.media.AudioAttributes
import android.media.AudioFormat
import android.media.AudioTrack
import android.util.Log
import java.io.Closeable
import kotlin.math.PI
import kotlin.math.sin

/** Two-second rising/falling siren loop on the alarm audio route. */
class SosSiren : Closeable {
    private var track: AudioTrack? = null

    @Synchronized fun setActive(active: Boolean) {
        if (!active) {
            close()
            return
        }
        if (track != null) return
        try {
            val sampleRate = 22_050
            val sampleCount = sampleRate * 2
            val pcm = ByteArray(sampleCount * 2)
            var phase = 0.0
            for (i in 0 until sampleCount) {
                val half = i % sampleRate
                val fraction = half.toDouble() / sampleRate
                val frequency = if (i < sampleRate) 650.0 + 450.0 * fraction
                                else 1100.0 - 450.0 * fraction
                phase += 2.0 * PI * frequency / sampleRate
                val ramp = minOf(i, sampleCount - 1 - i, 220).toDouble() / 220.0
                val value = (sin(phase) * 12000.0 * ramp).toInt()
                pcm[i * 2] = value.toByte()
                pcm[i * 2 + 1] = (value shr 8).toByte()
            }
            val created = AudioTrack.Builder()
                .setAudioAttributes(AudioAttributes.Builder()
                    .setUsage(AudioAttributes.USAGE_ALARM)
                    .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                    .build())
                .setAudioFormat(AudioFormat.Builder()
                    .setEncoding(AudioFormat.ENCODING_PCM_16BIT)
                    .setSampleRate(sampleRate)
                    .setChannelMask(AudioFormat.CHANNEL_OUT_MONO)
                    .build())
                .setTransferMode(AudioTrack.MODE_STATIC)
                .setBufferSizeInBytes(pcm.size)
                .build()
            if (created.write(pcm, 0, pcm.size) != pcm.size ||
                created.setLoopPoints(0, sampleCount, -1) != AudioTrack.SUCCESS) {
                created.release()
                return
            }
            created.play()
            track = created
        } catch (error: RuntimeException) {
            Log.w("FallSafe/SIREN", "Không phát được còi SOS", error)
            close()
        }
    }

    @Synchronized override fun close() {
        val previous = track ?: return
        track = null
        try { previous.stop() } catch (_: IllegalStateException) {}
        previous.release()
    }
}
