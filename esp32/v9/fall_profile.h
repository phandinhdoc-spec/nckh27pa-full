#pragma once

#include <math.h>
#include <stdint.h>

struct CauHinhNga {
  // Mỗi ngưỡng là một thành phần riêng để Android có thể điều chỉnh bộ thông số qua BLE.
  float nguongVaDapMs2 = 25.0f; // Gia tốc tối thiểu để xem một mẫu là va đập mạnh.
  float giaTocNamYenMs2 = 9.81f; // Độ lớn gia tốc khi đứng hoặc nằm yên, xấp xỉ gia tốc trọng trường.
  float saiSoNamYenMs2 = 1.0f; // Độ lệch cho phép quanh mức gia tốc nằm yên để chấp nhận mẫu.
  uint32_t cuaSoSauVaDapMs = 3000; // Thời hạn tối đa sau va đập để hoàn tất các điều kiện xác minh.
  uint32_t thoiGianNamYenSauVaDapMs = 1000; // Thời gian người dùng cần nằm yên liên tục trước khi xác nhận.
  uint16_t soMauNamYenToiThieu = 6; // Ngăn xác nhận khi có quá ít mẫu nằm yên.
  uint32_t khoangDutMauToiDaMs = 250; // Nếu mất mẫu lâu hơn mức này, bỏ chuỗi bằng chứng đang xét.
  float nguongRoiTuDoMs2 = 4.9f; // Gia tốc dưới ngưỡng này bắt đầu một ứng viên rơi tự do.
  uint32_t thoiGianRoiTuDoToiThieuMs = 80; // Pha gia tốc thấp phải kéo dài ít nhất khoảng thời gian này.
  float nguongXoayManhDps = 120.0f; // Tốc độ xoay tối thiểu để ghi nhận động tác xoay hoặc lật người.
  float mucTangApSuatToiThieuPa = 12.0f; // Mức tăng áp suất tối thiểu để làm bằng chứng hỗ trợ.
  uint32_t cuaSoApSuatMs = 5000; // Khoảng thời gian tối đa được dùng bằng chứng áp suất.
  float mucGiamDoCaoToiThieuM = -0.40f; // Mức giảm độ cao tối thiểu để hỗ trợ xác nhận.
  uint32_t thoiHanMauMs = 100; // Tuổi tối đa của mẫu cảm biến trước khi xem dữ liệu là cũ.

  // Kiểm tra toàn bộ thông số trước khi lưu hoặc áp dụng, tránh giá trị ngoài miền làm sai logic hay thời gian chờ.
  bool hopLe() const {
    return isfinite(nguongVaDapMs2) && nguongVaDapMs2 >= 1 && nguongVaDapMs2 <= 100 &&
           isfinite(giaTocNamYenMs2) && giaTocNamYenMs2 >= 0 && giaTocNamYenMs2 <= 20 &&
           isfinite(saiSoNamYenMs2) && saiSoNamYenMs2 >= 0.1f && saiSoNamYenMs2 <= 10 &&
           cuaSoSauVaDapMs >= 500 && cuaSoSauVaDapMs <= 10000 &&
           thoiGianNamYenSauVaDapMs >= 100 && thoiGianNamYenSauVaDapMs <= cuaSoSauVaDapMs &&
           soMauNamYenToiThieu >= 2 && soMauNamYenToiThieu <= 100 &&
           khoangDutMauToiDaMs >= 10 && khoangDutMauToiDaMs <= 2000 &&
           isfinite(nguongRoiTuDoMs2) && nguongRoiTuDoMs2 >= 0.1f && nguongRoiTuDoMs2 <= 20 &&
           thoiGianRoiTuDoToiThieuMs >= 20 && thoiGianRoiTuDoToiThieuMs <= 1000 &&
           isfinite(nguongXoayManhDps) && nguongXoayManhDps >= 1 && nguongXoayManhDps <= 2000 &&
           isfinite(mucTangApSuatToiThieuPa) && mucTangApSuatToiThieuPa >= 1 && mucTangApSuatToiThieuPa <= 1000 &&
           cuaSoApSuatMs >= 100 && cuaSoApSuatMs <= 10000 &&
           isfinite(mucGiamDoCaoToiThieuM) && mucGiamDoCaoToiThieuM >= -10 && mucGiamDoCaoToiThieuM <= -0.01f &&
           thoiHanMauMs >= 20 && thoiHanMauMs <= 2000;
  }
};
