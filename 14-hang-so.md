# Ý nghĩa 14 tham số phát hiện té ngã

## Phạm vi và nguồn

“14 hằng số” trong báo cáo là **14 tham số cấu hình** của `BleFallProfile`. Chúng có thể điều chỉnh khi hiệu chuẩn, không phải các hằng số vật lý bất biến.

Nguồn giá trị mặc định đã đối chiếu trực tiếp: `../nckh27pa/nckh27pa/android/app/src/main/java/vn/nckh27pa/fallsafe/bluetooth/BleProfilePayload.kt`, phần khai báo `BleFallProfile` (dòng 10–24). Các giá trị dưới đây là mặc định trong mã Android, **không phải xác nhận cấu hình đang chạy trên thiết bị**.

## 1. Va đập và xác minh bất động

| STT | Tên trong mã | Mặc định | Ý nghĩa |
| --- | --- | --- | --- |
| 1 | `impactAccelerationMs2` | 25 m/s² | Ngưỡng va đập. Khi độ lớn gia tốc tổng đạt hoặc vượt ngưỡng, hệ thống ghi nhận va đập đáng nghi và bắt đầu xác minh; chưa kết luận ngã ngay. |
| 2 | `stillnessTargetAccelerationMs2` | 9,81 m/s² | Gia tốc mục tiêu khi thiết bị đứng yên. Cảm biến vẫn đo tác dụng của trọng lực nên giá trị mong đợi gần 9,81 m/s², không phải 0. |
| 3 | `stillnessToleranceMs2` | 1 m/s² | Dung sai quanh gia tốc mục tiêu. Mẫu đạt tiêu chí tĩnh khi độ lớn gia tốc tổng lệch khỏi mục tiêu không quá dung sai. Tiêu chí này không bảo đảm cơ thể hoàn toàn bất động. |
| 4 | `postImpactWindowMs` | 3000 ms | Cửa sổ xác minh sau va đập: khoảng thời gian cho phép tìm một đoạn tĩnh đạt yêu cầu. Hết cửa sổ mà chưa đủ điều kiện thì không xác nhận ngã theo nhánh này. |
| 5 | `postImpactStillnessDurationMs` | 1000 ms | Thời gian tĩnh liên tục tối thiểu. Phân biệt đoạn tĩnh kéo dài với vài mẫu tĩnh thoáng qua. |
| 6 | `minimumStillnessSamples` | 6 mẫu | Số mẫu tĩnh tối thiểu. Phải đạt đồng thời số mẫu này và thời gian tại mục 5; đủ 6 mẫu riêng lẻ chưa đủ để kết luận. |
| 7 | `maximumSampleGapMs` | 250 ms | Khe thời gian lớn nhất giữa hai mẫu trong quá trình xác minh. Dữ liệu gián đoạn quá lâu làm mất căn cứ khẳng định đoạn tĩnh liên tục. |

Độ lớn gia tốc tổng được tính từ ba trục:

```text
a = sqrt(ax² + ay² + az²)
```

Điều kiện một mẫu đạt tiêu chí tĩnh:

```text
abs(a - stillnessTargetAccelerationMs2) <= stillnessToleranceMs2
```

## 2. Dấu hiệu rơi, xoay và thay đổi độ cao

| STT | Tên trong mã | Mặc định | Ý nghĩa |
| --- | --- | --- | --- |
| 8 | `freeFallThresholdMs2` | 4,90 m/s² | Ngưỡng nghi ngờ rơi tự do. Khi cơ thể rơi, số đo gia tốc của cảm biến có thể giảm mạnh. Số đo thấp không có nghĩa gia tốc trọng trường thực tế giảm đi. |
| 9 | `freeFallMinDurationMs` | 80 ms | Thời gian tối thiểu của dấu hiệu rơi tự do, nhằm phân biệt một đoạn gia tốc thấp kéo dài với một mẫu nhiễu đơn lẻ. |
| 10 | `gyroTurnThresholdDps` | 120 độ/giây | Ngưỡng tốc độ xoay nhanh của thiết bị/cơ thể. Đây là tốc độ góc, không phải góc nghiêng 120 độ. |
| 11 | `pressureEvidenceMinRisePa` | 12 Pa | Mức tăng áp suất tối thiểu để ghi nhận bằng chứng hạ thấp độ cao. Khi thiết bị xuống thấp, áp suất khí quyển thường tăng; cần xét nhiễu và điều kiện môi trường. |
| 12 | `pressureWindowMs` | 5000 ms | Cửa sổ thời gian xem xét bằng chứng thay đổi áp suất liên quan đến chuyển động ngã. Khác cửa sổ xác minh bất động ở mục 4. |
| 13 | `altitudeDropMinM` | −0,40 m | Ngưỡng thay đổi độ cao theo hướng đi xuống. Dấu âm biểu thị độ cao giảm; tương ứng dấu hiệu hạ thấp ít nhất 0,40 m. |

Các mục này diễn giải ý nghĩa của trường cấu hình. Việc chúng có được sử dụng, sử dụng làm điều kiện bắt buộc hay chỉ làm thông tin bổ sung phụ thuộc thuật toán thực sự đang chạy.

## 3. Giám sát dữ liệu cảm biến

| STT | Tên trong mã | Mặc định | Ý nghĩa |
| --- | --- | --- | --- |
| 14 | `sampleWatchdogMs` | 100 ms | Thời hạn giám sát việc có mẫu mới, dùng để nhận biết dòng dữ liệu bị chậm hoặc ngừng. Không phải tần số lấy mẫu và không có nghĩa cảm biến chỉ đo một lần mỗi 100 ms. |

Phân biệt hai tham số liên quan đến thời gian mẫu:

- `maximumSampleGapMs`: ràng buộc sự liên tục của dữ liệu trong quá trình xác minh sau va đập.
- `sampleWatchdogMs`: tham số giám sát độ mới của dòng dữ liệu; cần kiểm tra nơi sử dụng để xác nhận hành vi cụ thể trên firmware.

## 4. Cách diễn đạt nguyên lý chính

> Có va đập mạnh, sau đó trong cửa sổ xác minh xuất hiện đoạn gia tốc gần trọng trường, kéo dài đủ thời gian, đủ số mẫu và không bị gián đoạn dữ liệu thì hệ thống xác nhận ngã theo nhánh va đập–tĩnh.

Không nên diễn đạt rằng cả 14 tham số đều là điều kiện bắt buộc để kết luận ngã. Các dấu hiệu rơi tự do, xoay nhanh và thay đổi độ cao có thể là thông tin bổ sung, tùy phiên bản thuật toán.

## 5. Lưu ý khi dùng trong báo cáo

Bản firmware đã đọc tại `../nckh27pa/nckh27pa/esp-s3/esp-s3.ino` chưa khớp hoàn toàn với bộ 14 trường của `BleFallProfile`:

- `PROFILE_DEFAULT` trong firmware đặt ngưỡng rơi tự do là **3 m/s²** và ngưỡng tốc độ góc cao là **200 độ/giây**, khác **4,90 m/s²** và **120 độ/giây** của bộ cấu hình Android nêu trên.
- Nhánh `processFallDetection` bắt đầu xác minh khi phát hiện **va đập**, rồi kiểm tra **đoạn tĩnh**. Nhánh này không bắt buộc phải có rơi tự do trước va đập.
- Trong nhánh đã đọc, dấu hiệu rơi tự do được dùng cho thông tin sự kiện và điểm tin cậy; xoay nhanh được ghi vào lý do kích hoạt. Không thể suy ra mọi trường cấu hình Android đều đang tham gia quyết định trên firmware.

Vì vậy, trước khi khẳng định “toàn bộ 14 hằng số được chuyển xuống và áp dụng trên thiết bị”, cần đối chiếu đường truyền cấu hình, nơi áp dụng từng trường và firmware thực sự đang nạp. Một phản hồi lệnh thành công riêng lẻ chưa chứng minh đủ 14 trường đã được thuật toán sử dụng.

Đây là tài liệu giải thích tham số của mẫu thử, không phải bằng chứng về độ chính xác phát hiện ngã hay khả năng chẩn đoán đột quỵ. Khi hiệu chỉnh ngưỡng, cần dựa trên dữ liệu thử nghiệm thực tế và kiểm tra cả bỏ sót ngã lẫn báo động giả.
