#include "../fall_detector.h"

#include <assert.h>

static void lapQuaKhoangMau(BoPhatHienNga &boPhatHien, uint32_t dauTien, uint32_t cuoiCung,
                      float giaToc, float xoay, float doCaoM,
                      bool hopLe = true) {
  for (uint32_t thoiDiem = dauTien; thoiDiem <= cuoiCung; thoiDiem += 10)
    boPhatHien.xuLyMau(thoiDiem, giaToc, xoay, doCaoM, hopLe);
}

int main() {
  BoPhatHienNga lanNga;
  lapQuaKhoangMau(lanNga, 10, 100, 9.81f, 0, 0);
  lapQuaKhoangMau(lanNga, 110, 190, 2.0f, 0, 0);
  assert(lanNga.trangThai == TrangThaiNga::NGHI_NGO);
  lanNga.xuLyMau(200, 30.0f, 150.0f, -0.2f, true);
  assert(lanNga.trangThai == TrangThaiNga::XAC_MINH);
  lapQuaKhoangMau(lanNga, 210, 1200, 9.81f, 0, -0.55f);
  assert(lanNga.trangThai == TrangThaiNga::XAC_MINH);
  assert(lanNga.xuLyMau(1210, 9.81f, 0, -0.55f, true));
  assert(lanNga.trangThai == TrangThaiNga::CANH_BAO && lanNga.maSuKien == 1);
  assert(!lanNga.xuLyMau(1220, 9.81f, 0, -0.55f, true));

  BoPhatHienNga lanNgaNgan;
  lapQuaKhoangMau(lanNgaNgan, 10, 70, 2.0f, 0, 0);
  lanNgaNgan.xuLyMau(80, 30.0f, 150.0f, -0.6f, true);
  assert(lanNgaNgan.trangThai == TrangThaiNga::THEO_DOI);

  BoPhatHienNga khongXoay;
  lapQuaKhoangMau(khongXoay, 10, 100, 2.0f, 0, 0);
  khongXoay.xuLyMau(110, 30.0f, 0, -0.6f, true);
  assert(khongXoay.trangThai == TrangThaiNga::NGHI_NGO);

  BoPhatHienNga khongHaDoCao;
  lapQuaKhoangMau(khongHaDoCao, 10, 100, 2.0f, 0, 0);
  khongHaDoCao.xuLyMau(110, 30.0f, 150.0f, 0, true);
  lapQuaKhoangMau(khongHaDoCao, 120, 1200, 9.81f, 0, -0.2f);
  assert(khongHaDoCao.trangThai == TrangThaiNga::XAC_MINH);

  BoPhatHienNga dutMau;
  CauHinhNga cauHinhKhe;
  cauHinhKhe.khoangDutMauToiDaMs = 25;
  dutMau.apDungCauHinh(cauHinhKhe);
  lapQuaKhoangMau(dutMau, 10, 100, 2.0f, 0, 0);
  dutMau.xuLyMau(140, 30.0f, 150.0f, -0.6f, true);
  assert(dutMau.trangThai == TrangThaiNga::THEO_DOI);

  BoPhatHienNga thieuKhiApKe;
  lapQuaKhoangMau(thieuKhiApKe, 10, 100, 2.0f, 0, 0);
  thieuKhiApKe.xuLyMau(110, 30.0f, 150.0f, -0.6f, false);
  assert(thieuKhiApKe.trangThai == TrangThaiNga::THEO_DOI);

  CauHinhNga cauHinhTuyChinh;
  cauHinhTuyChinh.nguongVaDapMs2 = 40.0f;
  assert(cauHinhTuyChinh.hopLe());
  BoPhatHienNga boDaTinhChinh;
  boDaTinhChinh.apDungCauHinh(cauHinhTuyChinh);
  lapQuaKhoangMau(boDaTinhChinh, 10, 100, 2.0f, 0, 0);
  boDaTinhChinh.xuLyMau(110, 30.0f, 150.0f, -0.6f, true);
  assert(boDaTinhChinh.trangThai == TrangThaiNga::NGHI_NGO);
  boDaTinhChinh.xuLyMau(120, 45.0f, 150.0f, -0.6f, true);
  assert(boDaTinhChinh.trangThai == TrangThaiNga::XAC_MINH);

  BoPhatHienNga ngaCoApSuat;
  for (uint32_t thoiDiem = 10; thoiDiem <= 100; thoiDiem += 10)
    ngaCoApSuat.xuLyMau(thoiDiem, 2.0f, 0, 0, true, 100000.0f);
  ngaCoApSuat.xuLyMau(110, 30.0f, 150.0f, 0, true, 100000.0f);
  bool apSuatXacNhan = false;
  for (uint32_t thoiDiem = 120; thoiDiem <= 1120; thoiDiem += 10)
    apSuatXacNhan |= ngaCoApSuat.xuLyMau(thoiDiem, 9.81f, 0, 0, true, 100015.0f);
  assert(apSuatXacNhan && ngaCoApSuat.trangThai == TrangThaiNga::CANH_BAO);

  CauHinhNga cauHinhSai = cauHinhTuyChinh;
  cauHinhSai.thoiGianNamYenSauVaDapMs = cauHinhSai.cuaSoSauVaDapMs + 1;
  assert(!cauHinhSai.hopLe());
}
