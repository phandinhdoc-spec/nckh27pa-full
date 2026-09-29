#pragma once

#include <math.h>
#include <stdint.h>
#include "fall_profile.h"

// Các giai đoạn nhận dạng: theo dõi, nghi ngờ, xác minh và cảnh báo.
enum class TrangThaiNga : uint8_t { THEO_DOI, NGHI_NGO, XAC_MINH, CANH_BAO };

struct BoPhatHienNga {
  CauHinhNga cauHinh; // Các ngưỡng và khoảng thời gian dùng để đánh giá mẫu cảm biến.
  TrangThaiNga trangThai = TrangThaiNga::THEO_DOI; // Lưu giai đoạn hiện tại qua nhiều lần gọi xuLyMau().
  uint16_t maSuKien = 0; // Tạo mã riêng cho mỗi lần ngã được xác nhận để Android nhận biết sự kiện.
  uint32_t soLanDutMau = 0; // Đếm những khoảng mất mẫu bất thường, giúp phát hiện dữ liệu không liên tục.
  uint32_t lucLayMauTruocMs = 0; // So với thời điểm hiện tại để tính khoảng cách và phát hiện mất mẫu.
  uint32_t lucBatDauRoiTuDoMs = 0; // Mốc dùng để tính thời gian rơi và giới hạn giai đoạn nghi ngã.
  uint32_t lucXoayManhMs = 0; // Ghi nhận thời điểm xoay mạnh gần lúc va đập.
  uint32_t lucVaDapMs = 0; // Mốc bắt đầu theo dõi trạng thái sau va đập.
  uint32_t lucBatDauNamYenMs = 0; // Mốc tính thời gian nằm yên liên tục sau va đập.
  float doCaoThamChieuM = 0; // Đường nền được lọc chậm để so sánh độ cao hiện tại với trước khi ngã.
  float doCaoTruocNgaM = 0; // Độ cao lúc bắt đầu rơi, dùng để tính mức thay đổi sau ngã.
  float doThayDoiDoCaoM = 0; // Độ cao hiện tại trừ độ cao trước ngã, hỗ trợ xác nhận ngã.
  float apSuatTruocNgaPa = NAN; // Áp suất lúc bắt đầu nghi ngã; NAN nghĩa là khi đó chưa có mẫu áp kế.
  uint16_t soMauNamYen = 0; // Đếm mẫu liên tiếp gần 1g để tránh xác nhận chỉ từ một mẫu đơn lẻ.
  bool daCoMau = false; // Phân biệt chưa có mốc mẫu với mốc millis() bằng 0.
  bool daCoDoCaoThamChieu = false; // Cho biết đường nền độ cao đã được khởi tạo.
  bool dangRoiTuDo = false; // Lưu trạng thái đang ở pha gia tốc thấp.
  bool daThayXoayManh = false; // Ghi nhớ bằng chứng xoay trước hoặc gần lúc va đập.
  bool dangNamYen = false; // Lưu mốc bắt đầu và đặt lại bộ đếm nếu người dùng cử động.

  // Xóa dấu hiệu của lần nghi ngã đang xét; giữ nguyên trạng thái CANH_BAO.
  // Đặt lại đồng thời các mốc và cờ để không dùng lẫn bằng chứng của hai lần ngã khác nhau.
  void xoaUngVienNga() {
    if (trangThai != TrangThaiNga::CANH_BAO) trangThai = TrangThaiNga::THEO_DOI;
    daCoDoCaoThamChieu = false;
    dangRoiTuDo = daThayXoayManh = dangNamYen = false;
    doThayDoiDoCaoM = 0;
    apSuatTruocNgaPa = NAN;
    soMauNamYen = 0;
  }

  // Thay bộ ngưỡng và xóa mẫu cũ để không trộn lẫn hai bộ cấu hình.
  // Cấu hình mới có thể đổi điều kiện xác nhận nên mốc mẫu trước đó không còn đáng tin cậy.
  void apDungCauHinh(const CauHinhNga &cauHinhMoi) {
    cauHinh = cauHinhMoi;
    xoaUngVienNga();
    daCoMau = false;
  }

  // Xử lý một bộ mẫu mới; chỉ trả về đúng một lần khi chuyển sang CANH_BAO.
  // Tham số gồm thời điểm, độ lớn gia tốc và tốc độ xoay, độ cao, trạng thái cảm biến và áp suất (nếu có).
  bool xuLyMau(uint32_t hienTaiMs, float doLonGiaTocMs2, float doLonXoayDps,
            float doCaoM, bool camBienHopLe, float apSuatPa = NAN) {
    if (trangThai == TrangThaiNga::CANH_BAO) return false;
    if (!camBienHopLe || !isfinite(doLonGiaTocMs2) || !isfinite(doLonXoayDps) ||
        !isfinite(doCaoM)) {
      xoaUngVienNga();
      daCoMau = false;
      return false;
    }
    if (daCoMau && uint32_t(hienTaiMs - lucLayMauTruocMs) > cauHinh.khoangDutMauToiDaMs) {
      ++soLanDutMau;
      xoaUngVienNga();
    }
    lucLayMauTruocMs = hienTaiMs;
    daCoMau = true;

    if (!daCoDoCaoThamChieu) {
      doCaoThamChieuM = doCaoM;
      daCoDoCaoThamChieu = true;
    }
    if (doLonXoayDps > cauHinh.nguongXoayManhDps) {
      daThayXoayManh = true;
      lucXoayManhMs = hienTaiMs;
    }

    if (trangThai == TrangThaiNga::THEO_DOI) {
      if (doLonGiaTocMs2 < cauHinh.nguongRoiTuDoMs2) {
        if (!dangRoiTuDo) {
          dangRoiTuDo = true;
          lucBatDauRoiTuDoMs = hienTaiMs;
          doCaoTruocNgaM = doCaoThamChieuM;
          apSuatTruocNgaPa = apSuatPa;
        }
        if (uint32_t(hienTaiMs - lucBatDauRoiTuDoMs) >= cauHinh.thoiGianRoiTuDoToiThieuMs) trangThai = TrangThaiNga::NGHI_NGO;
      } else {
        dangRoiTuDo = false;
        doCaoThamChieuM += 0.02f * (doCaoM - doCaoThamChieuM);
      }
    }

    if (trangThai == TrangThaiNga::NGHI_NGO) {
      if (uint32_t(hienTaiMs - lucBatDauRoiTuDoMs) > 1500) {
        xoaUngVienNga();
        return false;
      }
      if (doLonGiaTocMs2 >= cauHinh.nguongVaDapMs2 && daThayXoayManh &&
          uint32_t(hienTaiMs - lucXoayManhMs) <= 150) {
        trangThai = TrangThaiNga::XAC_MINH;
        lucVaDapMs = hienTaiMs;
        dangNamYen = false;
        soMauNamYen = 0;
      }
    } else if (trangThai == TrangThaiNga::XAC_MINH) {
      if (uint32_t(hienTaiMs - lucVaDapMs) > cauHinh.cuaSoSauVaDapMs) {
        xoaUngVienNga();
        return false;
      }
      doThayDoiDoCaoM = doCaoM - doCaoTruocNgaM;
      if (fabsf(doLonGiaTocMs2 - cauHinh.giaTocNamYenMs2) <= cauHinh.saiSoNamYenMs2) {
        if (!dangNamYen) {
          dangNamYen = true;
          lucBatDauNamYenMs = hienTaiMs;
          soMauNamYen = 0;
        }
        if (soMauNamYen < UINT16_MAX) ++soMauNamYen;
        // Áp suất bổ sung bằng chứng về độ cao; chỉ dùng khi cả hai mẫu áp suất hợp lệ và còn trong khoảng cho phép.
        bool coBangChungApSuat = isfinite(apSuatPa) && isfinite(apSuatTruocNgaPa) &&
                                uint32_t(hienTaiMs - lucBatDauRoiTuDoMs) <= cauHinh.cuaSoApSuatMs &&
                                apSuatPa - apSuatTruocNgaPa >= cauHinh.mucTangApSuatToiThieuPa;
        if (uint32_t(hienTaiMs - lucBatDauNamYenMs) >= cauHinh.thoiGianNamYenSauVaDapMs &&
            soMauNamYen >= cauHinh.soMauNamYenToiThieu &&
            (doThayDoiDoCaoM <= cauHinh.mucGiamDoCaoToiThieuM || coBangChungApSuat)) {
          trangThai = TrangThaiNga::CANH_BAO;
          if (++maSuKien == 0) ++maSuKien;
          return true;
        }
      } else {
        dangNamYen = false;
        soMauNamYen = 0;
      }
    }
    return false;
  }
};
