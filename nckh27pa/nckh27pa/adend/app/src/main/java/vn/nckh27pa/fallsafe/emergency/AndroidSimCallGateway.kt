package vn.nckh27pa.fallsafe.emergency

import android.Manifest
import android.app.Notification
import android.content.ActivityNotFoundException
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.drawable.Icon
import android.net.Uri
import android.os.Handler
import android.os.Looper
import android.util.Log
import androidx.core.content.ContextCompat
import java.util.concurrent.CountDownLatch
import java.util.concurrent.TimeUnit
import java.util.concurrent.atomic.AtomicReference
import vn.nckh27pa.fallsafe.permissions.resolveTelephonySupport

class AndroidSimCallGateway(private val context: Context) : EmergencyCallGateway {
    override fun call(phone: String): CallDispatchState {
        val state = try {
            when {
                ContextCompat.checkSelfPermission(context, Manifest.permission.CALL_PHONE) != PackageManager.PERMISSION_GRANTED -> {
                    // TEMPORARY DIAGNOSTIC
                    vn.nckh27pa.fallsafe.AndroidTrace.logBlocked("CALL", "checkSelfPermission(CALL_PHONE)!=PERMISSION_GRANTED")
                    vn.nckh27pa.fallsafe.AndroidTrace.logCall(entered = true, contactExists = true, phonePresent = phone.isNotBlank(), permission = false, intentCreated = false, startActivityReached = false, startActivityReturned = false, exception = "none")
                    CallDispatchState(CallStatus.PERMISSION_MISSING, EmergencyFailureMessages.callPermissionMissing)
                }
                !resolveTelephonySupport(
                    hasBaseTelephony = context.packageManager.hasSystemFeature(PackageManager.FEATURE_TELEPHONY),
                    hasGranularFeature = context.packageManager.hasSystemFeature(PackageManager.FEATURE_TELEPHONY_CALLING)
                ) -> {
                    // TEMPORARY DIAGNOSTIC
                    vn.nckh27pa.fallsafe.AndroidTrace.logBlocked("CALL", "FEATURE_TELEPHONY_CALLING=false")
                    vn.nckh27pa.fallsafe.AndroidTrace.logCall(entered = true, contactExists = true, phonePresent = phone.isNotBlank(), permission = ContextCompat.checkSelfPermission(context, Manifest.permission.CALL_PHONE) == PackageManager.PERMISSION_GRANTED, intentCreated = false, startActivityReached = false, startActivityReturned = false, exception = "none")
                    CallDispatchState(CallStatus.UNAVAILABLE, "Thiết bị không hỗ trợ cuộc gọi SIM.")
                }
                phone.isBlank() -> {
                    // TEMPORARY DIAGNOSTIC
                    vn.nckh27pa.fallsafe.AndroidTrace.logBlocked("CALL", "phone_blank")
                    vn.nckh27pa.fallsafe.AndroidTrace.logCall(entered = true, contactExists = true, phonePresent = false, permission = true, intentCreated = false, startActivityReached = false, startActivityReturned = false, exception = "none")
                    CallDispatchState(CallStatus.UNAVAILABLE, "Chưa có số điện thoại để gọi.")
                }
                else -> {
                    val encodedPhone = Uri.encode(phone)
                    val intent = Intent(Intent.ACTION_CALL, Uri.parse("tel:$encodedPhone")).addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
                    launch(intent, phone)
                }
            }
        } catch (_: SecurityException) {
            CallDispatchState(CallStatus.PERMISSION_MISSING, "Hệ thống từ chối quyền gọi điện.")
        } catch (_: ActivityNotFoundException) {
            CallDispatchState(CallStatus.FAILED, "Không có ứng dụng gọi điện.")
        } catch (_: RuntimeException) {
            CallDispatchState(CallStatus.UNAVAILABLE, "Không thể mở cuộc gọi SIM lúc này.")
        }
        log(state)
        return state
    }

    private fun launch(intent: Intent, phone: String): CallDispatchState {
        // An emergency notification with a "+ Gọi ngay" action always works, including from the
        // background where Android blocks startActivity(ACTION_CALL) despite CALL_PHONE being granted.
        postEmergencyCallNotification(phone)
        val fallback = CallDispatchState(CallStatus.STARTED, "Đã mở cảnh báo gọi khẩn cấp; bấm nút Gọi trong thông báo để quay số.")
        if (Looper.myLooper() == Looper.getMainLooper()) {
            return dial(intent, fallback)
        }
        return try {
            val completed = CountDownLatch(1)
            val launched = AtomicReference<CallDispatchState>()
            val handler = Handler(Looper.getMainLooper())
            val runnable = Runnable { launched.set(dial(intent, fallback)); completed.countDown() }
            if (!handler.post(runnable)) CallDispatchState(CallStatus.FAILED, "Không thể gửi yêu cầu gọi SIM tới luồng chính.")
            else if (completed.await(2, TimeUnit.SECONDS)) launched.get() ?: fallback
            else {
                handler.removeCallbacks(runnable)
                CallDispatchState(CallStatus.FAILED, "Hết thời gian chờ mở cuộc gọi SIM.")
            }
        } catch (_: RuntimeException) {
            CallDispatchState(CallStatus.FAILED, "Không thể gửi yêu cầu gọi SIM tới luồng chính.")
        } catch (_: InterruptedException) {
            Thread.currentThread().interrupt()
            CallDispatchState(CallStatus.FAILED, "Đã gián đoạn khi mở cuộc gọi SIM.")
        }
    }

    private fun dial(intent: Intent, fallback: CallDispatchState): CallDispatchState = try {
        // TEMPORARY DIAGNOSTIC
        vn.nckh27pa.fallsafe.AndroidTrace.logPermission(context)
        vn.nckh27pa.fallsafe.AndroidTrace.logCall(entered = true, contactExists = true, phonePresent = true, permission = true, intentCreated = true, startActivityReached = true, startActivityReturned = false, exception = "none")
        context.startActivity(intent)
        vn.nckh27pa.fallsafe.AndroidTrace.logCall(entered = true, contactExists = true, phonePresent = true, permission = true, intentCreated = true, startActivityReached = true, startActivityReturned = true, exception = "none")
        CallDispatchState(CallStatus.STARTED, "Đã mở cuộc gọi SIM.")
    } catch (e: ActivityNotFoundException) {
        // TEMPORARY DIAGNOSTIC
        vn.nckh27pa.fallsafe.AndroidTrace.logBlocked("CALL", "ActivityNotFoundException: ${e.message}")
        vn.nckh27pa.fallsafe.AndroidTrace.logCall(entered = true, contactExists = true, phonePresent = true, permission = true, intentCreated = true, startActivityReached = true, startActivityReturned = false, exception = e.javaClass.simpleName)
        CallDispatchState(CallStatus.FAILED, "Không có ứng dụng gọi điện.")
    } catch (e: SecurityException) {
        // Background activity start is blocked (Android 10+) even with CALL_PHONE; the notification is already posted.
        // TEMPORARY DIAGNOSTIC
        vn.nckh27pa.fallsafe.AndroidTrace.logBlocked("CALL", "SecurityException: ${e.message}")
        vn.nckh27pa.fallsafe.AndroidTrace.logCall(entered = true, contactExists = true, phonePresent = true, permission = true, intentCreated = true, startActivityReached = true, startActivityReturned = false, exception = e.javaClass.simpleName)
        fallback
    } catch (e: RuntimeException) {
        // Background activity start is blocked (Android 10+) even with CALL_PHONE; the notification is already posted.
        // TEMPORARY DIAGNOSTIC
        vn.nckh27pa.fallsafe.AndroidTrace.logBlocked("CALL", "RuntimeException: ${e.message}")
        vn.nckh27pa.fallsafe.AndroidTrace.logCall(entered = true, contactExists = true, phonePresent = true, permission = true, intentCreated = true, startActivityReached = true, startActivityReturned = false, exception = e.javaClass.simpleName)
        fallback
    }

    private fun postEmergencyCallNotification(phone: String) {
        val manager = context.getSystemService(Context.NOTIFICATION_SERVICE) as? NotificationManager ?: return
        manager.createNotificationChannel(
            NotificationChannel(EMERGENCY_CALL_CHANNEL, "Khẩn cấp", NotificationManager.IMPORTANCE_HIGH).apply {
                description = "Cảnh báo gọi khẩn cấp khi phát hiện SOS."
            }
        )
        val callIntent = Intent(Intent.ACTION_CALL, Uri.parse("tel:${Uri.encode(phone)}"))
            .addFlags(Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TOP)
        val callPending = PendingIntent.getActivity(
            context, phone.hashCode(), callIntent,
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
        )
        val builder = Notification.Builder(context, EMERGENCY_CALL_CHANNEL)
            .setSmallIcon(android.R.drawable.ic_dialog_alert)
            .setContentTitle("FallSafe — CẢNH BÁO KHẨN CẤP")
            .setContentText("Cần trợ giúp khẩn cấp. Bấm nút Gọi để quay số: $phone.")
            .setCategory(Notification.CATEGORY_CALL)
            .setPriority(Notification.PRIORITY_MAX)
            .setAutoCancel(true)
            .setContentIntent(callPending)
            .setFullScreenIntent(callPending, true)
            .addAction(
                Notification.Action.Builder(
                    Icon.createWithResource(context, android.R.drawable.ic_menu_call),
                    "Gọi ngay", callPending
                ).build()
            )
        manager.notify(EMERGENCY_CALL_NOTIFICATION_ID, builder.build())
    }

    private fun log(state: CallDispatchState) {
        if (state.status == CallStatus.STARTED) Log.i("FallSafe/CALL", "status=${state.status} detail=${state.detail}")
        else Log.w("FallSafe/CALL", "status=${state.status} detail=${state.detail}")
    }

    private companion object {
        const val EMERGENCY_CALL_CHANNEL = "emergency_call"
        const val EMERGENCY_CALL_NOTIFICATION_ID = 2001
    }
}