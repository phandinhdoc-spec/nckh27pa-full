package vn.nckh27pa.fallsafe.protocol

import com.google.gson.Strictness
import com.google.gson.stream.JsonReader
import com.google.gson.stream.JsonToken
import java.io.StringReader

/** A missing-IMU report is status, never a fabricated motion sample. */
data class Esp32SensorStatus(val deviceId: String, val timestampMs: Long, val pressurePa: Float?) {
    companion object {
        fun decodeUnavailable(json: String): Esp32SensorStatus? {
            if (json.length !in 2..4096) return null
            return try {
                val fields = mutableMapOf<String, Pair<JsonToken, String?>>()
                JsonReader(StringReader(json)).use { reader ->
                    reader.strictness = Strictness.STRICT
                    reader.beginObject()
                    while (reader.hasNext()) {
                        val key = reader.nextName()
                        require(!fields.containsKey(key))
                        val token = reader.peek()
                        val value = when (token) {
                            JsonToken.NULL -> { reader.nextNull(); null }
                            JsonToken.BOOLEAN -> reader.nextBoolean().toString()
                            JsonToken.STRING, JsonToken.NUMBER -> reader.nextString()
                            else -> error("Primitive status fields required")
                        }
                        fields[key] = token to value
                    }
                    reader.endObject()
                    require(reader.peek() == JsonToken.END_DOCUMENT)
                }
                fun integer(key: String): Long {
                    val field = requireNotNull(fields[key])
                    require(field.first == JsonToken.NUMBER)
                    val raw = requireNotNull(field.second)
                    require(raw.matches(Regex("0|[1-9][0-9]*")))
                    return raw.toLong()
                }
                require(integer("protocolVersion") == 1L && integer("sensorQuality") == 0L)
                integer("sequenceNumber")
                val timestamp = integer("timestampMs")
                val id = requireNotNull(fields["deviceId"])
                require(id.first == JsonToken.STRING && !id.second.isNullOrBlank() && id.second!!.length <= 128)
                for (key in listOf("accelXMs2", "accelYMs2", "accelZMs2", "gyroXDps", "gyroYDps", "gyroZDps")) {
                    require(fields[key]?.first == JsonToken.NULL)
                }
                for (key in listOf("isCharging", "sosButtonPressed")) require(fields[key]?.first == JsonToken.BOOLEAN)
                val pressure = fields["pressurePa"]?.let {
                    if (it.first == JsonToken.NULL) null else {
                        require(it.first == JsonToken.NUMBER)
                        requireNotNull(it.second?.toFloatOrNull()).also { p -> require(p.isFinite()) }
                    }
                }
                Esp32SensorStatus(requireNotNull(id.second), timestamp, pressure)
            } catch (_: Exception) { null }
        }
    }
}
