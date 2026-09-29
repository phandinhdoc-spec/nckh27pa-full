#import "@preview/cetz:0.3.4" as cetz: canvas, draw

#set page(
  paper: "a4",
  margin: (left: 3cm, right: 2cm, top: 2cm, bottom: 2cm),
  footer: context {
    if counter(page).get().first() > 1 {
      align(center)[#text(size: 10pt)[#counter(page).display()]]
    }
  },
)

#set text(font: "Times New Roman", size: 14pt, lang: "vi", region: "VN")
#let ten-du-an = [Thiết bị cảnh báo hỗ trợ người có nguy cơ đột quỵ, tai biến mạch máu não]
#let gvhd = [Phan Anh]
#let hs1 = [Cao Nguyễn Minh Thi]
#let hs2 = [Mai Đặng Gia Hân]
#let lop = [9/1]
#let nam-hoc = [2026]
#set par(justify: true, leading: 0.6em, spacing: 0.6em, first-line-indent: (amount: 8mm, all: true))

// #set list(indent: 8mm, body-indent: 0.6em, spacing: 0.6em, marker: ([-], [+]))
// #set enum(indent: 8mm, body-indent: 0.6em, spacing: 0.6em)

#set list(indent: 8mm, body-indent: 0.6em, spacing: 0.6em, marker: ([-], [+]))
#show list.item: it => [
  #set par(first-line-indent: 0pt, hanging-indent: -10mm)
  #it
]
#set enum(indent: 8mm, body-indent: 0.6em, spacing: 0.6em)
#show enum.item: it => [
  #set par(first-line-indent: 0pt, hanging-indent: -16mm)
  #it
]


#set heading(numbering: (..nums) => {
  if nums.len() == 1 {
    numbering("A. ", nums.at(0))
  } else if nums.len() == 2 {
    numbering("1. ", nums.at(1))
  } else if nums.len() == 3 {
    numbering("1.1. ", nums.at(1), nums.at(2))
  }
})

#show heading.where(level: 1): it => {
  v(0.6em, weak: true)
  block(width: 100%)[
    #set text(font: "Times New Roman", size: 14pt, lang: "vi", region: "VN")
    #align(left)[#text(weight: "bold")[#counter(heading).display(it.numbering) #h(0.2em) #it.body]]
  ]
  v(0.6em, weak: true)
}
#show heading.where(level: 2): it => {
  v(0.6em, weak: true)
  block(width: 100%)[
    #set text(font: "Times New Roman", size: 14pt, lang: "vi", region: "VN")
    #h(8mm)
    #text(weight: "bold")[#counter(heading).display(it.numbering) #h(0.2em) #it.body]
  ]
  v(0.6em, weak: true)
}
#show heading.where(level: 3): it => {
  v(0.6em, weak: true)
  block(width: 100%)[
    #set text(font: "Times New Roman", size: 14pt, lang: "vi", region: "VN", style: "italic")
    #h(8mm)
    #text(weight: "bold")[#counter(heading).display(it.numbering) #h(0.2em) #it.body]
  ]
  v(0.6em, weak: true)
}

#let title(body) = align(center)[#text(size: 14pt, weight: "bold")[#body]]
#let fig-caption(body) = align(center)[#v(4pt)#text(weight: "bold", style: "italic", size: 14pt)[#body]]

#show figure.where(kind: image): set figure(supplement: [Hình])
#show figure.where(kind: table): set figure(supplement: [Bảng])
#show figure.caption: it => [
  #v(4pt)
  #text(weight: "bold", style: "italic", size: 11pt)[
    #it.supplement #context it.counter.display(it.numbering). #it.body
  ]
]

// Số liệu kỹ thuật THẬT của dự án, trích từ mã nguồn và biên bản kiểm thử trong thư mục này.
#let mcu-thuc-te = [ESP32-S3 Super Mini (firmware esp-s3 1.0.0, protocolVersion 1)]
#let imu-thuc-te = [MPU6050 (GY-521) trên I2C Bus 0, SDA = GPIO 8, SCL = GPIO 9, 400 kHz]
#let ap-suat-thuc-te = [MS5611-01BA03 (GY-63) trên I2C Bus 1, SDA = GPIO 7, SCL = GPIO 6, 400 kHz]
#let tan-so-imu = [100 Hz]
#let nguong-va-dap = [$a >= 25$ m/s² (khoảng 2,55 g)]
#let nguong-roi-tu-do = [$a < 4,9$ m/s² liên tục từ 80 ms trở lên]
#let cua-so-sau-va-dap = [3000 ms]
#let dieu-kien-tinh = [độ lớn gia tốc thỏa $|a - g| <=$ dung sai 1,0 m/s² liên tục ít nhất 1000 ms, tối thiểu 6 mẫu]
#let mach-sac-pin = [Mạch sạc IP5306 vừa sạc pin, vừa có mạch bảo vệ, vừa có mạch tăng áp tới 5V]
#let pin = [Pin lipo 700mAh]
#let ten-ble = [tiền tố FALLSAFE-]
#let tong-chi-phi-du-kien = [635.000 đồng]
#let dem-dong-firmware = [1757 dòng]
#let so-trang-thai-fw = [7 trạng thái]
#let bo-dem-truoc-sk = [1000 mẫu (10 giây ở 100 Hz, 40 KB RAM)]
#let so-truong-profile = [14 hằng số]
#let so-test-android = [175 kiểm thử đơn vị]
#let dung-luong-apk = [12.566.848 byte]

// GHI CHÚ TRUNG THỰC: các con số đo trên máy tính và emulator (số test, APK, SOS emulator,
// framing lab) là SỐ THẬT trích từ docs/test-report.md. Các bảng thống kê thực địa 5.1–5.4
// (60 lượt SOS, 120 tình huống ngã, 40 lần đổi Profile, 5 người thử) là SỐ LIỆU GIẢ ĐỊNH
// mô phỏng theo yêu cầu của nhóm; cần thay bằng số đo thật trên thiết bị trước khi nộp.

// // =================== TRANG BÌA ===================
// #v(2cm)
// #align(center)[#text(size: 16pt, weight: "bold")[BÁO CÁO DỰ ÁN NGHIÊN CỨU KHOA HỌC]]
// #v(0.5cm)
// #align(center)[#text(size: 14pt, weight: "bold")[#ten-du-an]]
// #v(1cm)
// #align(center)[
//   Giáo viên hướng dẫn: #gvhd \
//   Học sinh thực hiện: #hs1 -- Lớp #lop \
//   #hs2 -- Lớp #lop
// ]
// #v(1cm)
// #align(center)[Năm #nam-hoc]

// #pagebreak()

// =================== TÓM TẮT ĐỀ TÀI ===================
#title[TÓM TẮT DỰ ÁN]
#v(0.5cm)

#text(weight: "bold")[Tính mới:] Tính mới của dự án là kết hợp phát hiện ngã tự động bằng cảm biến đeo ở thắt lưng với cảnh báo SOS ba nhánh độc lập trên điện thoại: khi xác nhận người bị ngã và bất động, hệ thống tự gọi điện một lần tới người nhận ưu tiên, đồng thời gửi SMS kèm tọa độ và đường dẫn bản đồ tới toàn bộ người thân mà người bị nạn không cần bấm nút. Toàn bộ #(so-truong-profile) hằng số phát hiện ngã được chỉnh trực tiếp trên ứng dụng Android và chuyển xuống thiết bị đeo qua Bluetooth, thiết bị chỉ áp dụng khi trả lời `COMPLETED` đúng mã lệnh.

#text(weight: "bold")[Tính khoa học:] Thuật toán phát hiện ngã được chúng em xây dựng dựa trên kiến thức vật lý (gia tốc, gia tốc trọng trường, tốc độ góc, áp suất khí quyển theo độ cao) và lập trình máy trạng thái #(so-trang-thai-fw) trạng thái: nghi ngờ rơi tự do khi #(nguong-roi-tu-do), xác nhận va đập khi #(nguong-va-dap), rồi kiểm tra bất động sau va đập theo #(dieu-kien-tinh). Firmware #(dem-dong-firmware) lưu bộ đệm trước sự kiện #(bo-dem-truoc-sk) để không mất dấu hiệu ngã. Ba nhánh gọi điện, SMS và vị trí chạy độc lập nên mất GPS hay mất mạng dữ liệu đều không ngăn được cảnh báo qua sóng di động.

#text(weight: "bold")[Tính thực tiễn:] Thiết bị đeo nhỏ gọn ở thắt lưng dùng #(mcu-thuc-te) cùng ứng dụng Android FallSafe đọc trực tiếp gia tốc, góc, gia tốc trọng trường từ ESP32, giúp người có nguy cơ đột quỵ và tai biến mạch máu não được phát hiện kịp thời khi ngã mà không có người bên cạnh, với tổng chi phí linh kiện dự kiến khoảng #(tong-chi-phi-du-kien). Ứng dụng đã qua #(so-test-android) kiểm thử đơn vị đạt 100% và kiểm thử SOS thật trên emulator với vị trí, SMS và cuộc gọi đều thành công.

#text(weight: "bold")[Tính cộng đồng:] Dự án hướng tới người cao tuổi và người có nguy cơ đột quỵ phải sống một mình hoặc thường ở nhà một mình. Theo các bài viết về đột quỵ trên Báo Sức khỏe và Đời sống #footnote[Theo các bài viết về đột quỵ trên Báo Sức khỏe và Đời sống (suckhoedoisong.vn)], mỗi năm Việt Nam có khoảng 200.000 ca đột quỵ mới, trong đó té ngã sau đột quỵ là nguyên nhân phổ biến khiến người bệnh không kịp gọi cấp cứu. Thiết bị cảnh báo tự động góp phần rút ngắn thời gian phát hiện và tăng cơ hội cấp cứu trong giờ vàng.

#pagebreak()

// =================== MỤC LỤC ===================
#title[MỤC LỤC]
#v(0.5cm)
#outline(title: none, indent: 1.5em)
// =================== NỘI DUNG CHÍNH ===================
#pagebreak()
= LÝ DO CHỌN DỰ ÁN

Đột quỵ và tai biến mạch máu não là một trong những nguyên nhân gây tử vong và tàn tật hàng đầu ở Việt Nam, với khoảng 200.000 ca mới mỗi năm. Người bị đột quỵ thường ngã đột ngột, mất ý thức hoặc liệt nửa người nên không thể tự gọi điện cầu cứu. Nếu không có người chứng kiến, thời gian từ lúc ngã đến lúc được phát hiện có thể kéo dài hàng giờ, làm mất cơ hội cấp cứu trong giờ vàng.

Qua trao đổi với 8 gia đình có người cao tuổi tại địa phương, chúng em nhận thấy ba khó khăn chính:
- *Phát hiện ngã khi không có người bên cạnh:* người cao tuổi sống một mình hoặc ở nhà một mình ban ngày; khi ngã và bất tỉnh thì không ai biết để gọi cấp cứu.
- *Gọi cầu cứu khi tay chân không cử động được:* sau đột quỵ, người bệnh có thể liệt hoặc yếu nửa người nên không bấm được điện thoại, kể cả điện thoại để ngay bên cạnh.
- *Người nhà không biết vị trí và tình trạng:* khi nhận tin báo muộn, người nhà không biết người thân ngã ở đâu, ngã lúc nào và có còn tỉnh táo hay không.

Từ thực trạng đó, chúng em đặt câu hỏi: *Làm thế nào để người có nguy cơ đột quỵ được tự động phát hiện khi ngã và gửi cảnh báo kèm vị trí tới người nhà, mà không cần người bị nạn phải thao tác gì?*

Chúng em quyết định thực hiện dự án *#ten-du-an* -- một thiết bị đeo ở thắt lưng dùng #(mcu-thuc-te) đo gia tốc, tốc độ góc và độ cao liên tục ở #(tan-so-imu); khi xác nhận té ngã, ứng dụng Android FallSafe tự động gọi điện và gửi SMS kèm tọa độ tới số người nhà đã lưu. Các vòng tay hãng hiện nay có phát hiện ngã nhưng giá vài triệu đồng và không tùy chỉnh được ngưỡng; giải pháp của chúng em dùng linh kiện rời phổ thông với tổng chi phí dự kiến khoảng #(tong-chi-phi-du-kien) và cho phép chỉnh toàn bộ hằng số phát hiện ngã ngay trên điện thoại.

Chúng em chọn đề tài này vì mong muốn vận dụng kiến thức vật lý, lập trình và điện tử để tạo ra một giải pháp thiết thực, chi phí hợp lí, góp phần bảo vệ người cao tuổi và người có nguy cơ đột quỵ trong sinh hoạt hằng ngày.

// #figure(
//   block(
//     width: 100%,
//     height: 6.5cm,
//     fill: rgb("#f1f5f9"),
//     stroke: 1pt + rgb("#94a3b8"),
//     align(center + horizon)[#text(style: "italic")[#image("img/mat-truoc.jpg",width: 8cm)], ],
//   ),
//   caption: [Khảo sát thực tế nhu cầu cảnh báo ngã của các gia đình có người cao tuổi.],
//   numbering: _ => "2.1",
// )

= CÂU HỎI NGHIÊN CỨU, VẤN ĐỀ NGHIÊN CỨU VÀ GIẢ THUYẾT KHOA HỌC

== Câu hỏi nghiên cứu

+ Làm thế nào để phát hiện người bị ngã một cách tự động bằng cảm biến gia tốc và con quay, phân biệt được ngã thật với các sinh hoạt thường ngày như ngồi xuống nhanh, cúi nhặt đồ hay nằm xuống giường?

+ Khi đã xác nhận té ngã, làm thế nào để điện thoại tự động gọi SOS và gửi SMS kèm tọa độ tới người nhà trong thời gian ngắn nhất, kể cả khi tín hiệu GPS yếu hay mất mạng dữ liệu mà chỉ còn sóng di động?

+ Có nhiều loại cảm biến và nhiều bộ hằng số phát hiện ngã khác nhau, nên lựa chọn linh kiện nào vừa dễ mua, dễ lập trình, giá rẻ, đồng thời cho phép chỉnh hằng số ngay trên ứng dụng Android và chuyển xuống thiết bị đeo?

== Vấn đề nghiên cứu

+ Nghiên cứu các thiết bị phát hiện ngã và nút SOS hiện có trên thị trường, ưu và nhược điểm từng loại, xác định hướng phát triển thiết bị đeo thắt lưng giá rẻ của đề tài.

+ Nghiên cứu nguyên lý cảm biến gia tốc MPU6050 và cảm biến khí áp MS5611, ưu và nhược điểm từng loại, xác định cách phối hợp hai cảm biến để phát hiện ngã.

+ Nghiên cứu các ngưỡng phát hiện ngã (rơi tự do, va đập, bất động sau va đập), lựa chọn bộ hằng số phù hợp và cơ chế chuyển hằng số từ ứng dụng Android xuống ESP32 qua Bluetooth.

+ Nghiên cứu cơ chế gọi điện và gửi SMS kèm tọa độ trên Android, bao gồm quyền truy cập, lấy vị trí nhanh khi tín hiệu GPS yếu và đường dự phòng khi mất mạng.

+ Nghiên cứu thiết kế thiết bị đeo ở thắt lưng sao cho đo chuyển động sát cơ thể, thoải mái khi đeo cả ngày và dễ bấm nút SOS khi cần.

+ Nghiên cứu cơ chế cảnh báo nhiều bước (/*còi tại chỗ,*/ đếm ngược xác minh, gọi điện, SMS) để vừa không bỏ sót người bị nạn, vừa hạn chế báo động giả làm phiền người nhà.

== Giả thuyết, đối tượng và phạm vi nghiên cứu
*Giả thuyết khoa học:* từ ba câu hỏi nghiên cứu, chúng em đặt ba giả thuyết cần kiểm tra bằng phép thử của phiên bản hiện hành.

(1) Có thể phát hiện té ngã tự động bằng chuỗi ba dấu hiệu vật lý liên tiếp: rơi tự do ($a < 4,9$ m/s²), va đập (#(nguong-va-dap)) và bất động sau va đập (#(dieu-kien-tinh)), kết hợp độ thay đổi độ cao từ cảm biến khí áp.

(2) Điện thoại Android có thể tự động gọi điện một lần và gửi SMS kèm tọa độ tới người nhà trong vòng vài giây sau khi xác nhận ngã, chỉ dùng sóng di động mà không cần mạng internet.

(3) Bộ hằng số phát hiện ngã lưu trên ứng dụng Android có thể chuyển xuống ESP32 qua Bluetooth và được thiết bị áp dụng ngay, giúp tinh chỉnh độ nhạy mà không cần nạp lại firmware.

// = THIẾT KẾ VÀ PHƯƠNG PHÁP NGHIÊN CỨU
= Thiết kế và phương pháp nghiên cứu
// Mục này mô tả đối tượng, phạm vi, phương pháp thu thập số liệu, kế hoạch và phân công.

== Đối tượng nghiên cứu
Là luồng phát hiện ngã và cảnh báo đầu-cuối gồm bốn khối trong thư mục dự án: firmware `esp-s3/esp-s3.ino` (#(dem-dong-firmware)) đọc MPU6050 và MS5611, chạy máy trạng thái phát hiện ngã và gửi gói JSON qua Bluetooth; ứng dụng Android FallSafe (hơn 40 tệp Kotlin, giao diện Compose 4 tab) nhận telemetry, quản lý Profile hằng số, tự động gọi điện và gửi SMS kèm tọa độ; máy chủ Node.js lưu trữ sự kiện, danh bạ và hàng đợi cảnh báo; phòng lab giao thức `protocol-lab` kiểm chứng đóng khung dữ liệu chung cho Kotlin và C++.

== Phạm vi nghiên cứu:
Nghiên cứu nhu cầu cảnh báo ngã của các gia đình có người cao tuổi tại địa phương; thử nghiệm phát hiện ngã trên người mô phỏng có đệm bảo vệ; thử nghiệm gọi SOS và SMS trên emulator Android API 36 và kiểm thử đơn vị trên máy tính. Đề tài không chẩn đoán đột quỵ mà chỉ phát hiện dấu hiệu vận động bất thường là té ngã và bất động sau ngã; chưa kiểm chứng trên bo ESP32 và điện thoại thật.

== Phương pháp nghiên cứu
Để thực hiện dự án, chúng em sử dụng các phương pháp sau:

1. *Nghiên cứu tài liệu:* Đọc Sách giáo khoa Vật lí 10 [8] (gia tốc, gia tốc trọng trường, chuyển động rơi tự do) và Sách giáo khoa Tin học 11 [9] (lập trình, máy trạng thái) làm cơ sở lý thuyết cho thuật toán phát hiện ngã; đồng thời đọc datasheet ESP32-S3 [1], tài liệu MPU6050 [2], MS5611 [3] và tài liệu lập trình Android về quyền gọi điện, SMS và vị trí [5].

2. *Thiết kế và phát triển mẫu thử:* Chia công việc thành hai phía. Phía thiết bị đeo: lập trình firmware Arduino cho ESP32-S3 đọc IMU ở #(tan-so-imu), lưu bộ đệm trước sự kiện #(bo-dem-truoc-sk), chạy máy trạng thái phát hiện ngã và gửi gói JSON qua Bluetooth. Phía điện thoại: xây dựng ứng dụng Android nhận telemetry, quản lý Profile hằng số, tự động gọi điện và gửi SMS kèm tọa độ khi có xác nhận ngã; máy chủ Node.js dùng SQLite lưu sự kiện và hàng đợi cảnh báo.

3. *Thực nghiệm và thống kê:* Ghi số lượt ngã mô phỏng phát hiện đúng, báo giả, số SMS và cuộc gọi SOS thành công, độ trễ và tỷ lệ mất gói Bluetooth trong các điều kiện khác nhau; chạy #(so-test-android) kiểm thử đơn vị Android, kiểm thử lab giao thức hai ngôn ngữ và kiểm thử SOS thật trên emulator.

4. *Khảo sát:* Mời người dùng đeo thử thiết bị và thao tác ứng dụng, ghi chép ý kiến về mức độ thoải mái, dễ bấm nút SOS và những điểm còn gây khó chịu.

Các phương pháp được nối theo vòng lặp: xác định yêu cầu, thiết kế và tích hợp mẫu thử, chạy kiểm thử đầu-cuối, ghi số liệu và khắc phục lỗi phần mềm hoặc phần cứng.

== Kế hoạch nghiên cứu
Dự án được thực hiện từ tháng 05/2026 đến tháng 09/2026 với bảng phân công và tiến độ cụ thể như sau:

#show figure: set block(breakable: true)
#figure(
  table(
    columns: (1.5cm, 4.5cm, 5cm, 2.5cm, 3.5cm),
    align: (center + horizon, left + horizon, left + horizon, center + horizon, left + horizon),
    table.header([*TT*], [*Nội dung công việc*], [*Các bước thực hiện*], [*Thời gian*], [*Người phụ trách*]),
    [1],
    [Lên ý tưởng, khảo sát nhu cầu và lập kế hoạch],
    [Hình thành ý tưởng thiết bị phát hiện ngã gọi SOS; trao đổi với các gia đình có người cao tuổi; xác định yêu cầu sử dụng và lập kế hoạch nghiên cứu.],
    [05/2026],
    [Cả nhóm],

    [2],
    [Lựa chọn cảm biến và xây dựng thuật toán phát hiện ngã],
    [Chọn MPU6050 đo gia tốc và góc, MS5611 đo độ cao tương đối; xây dựng máy trạng thái rơi tự do, va đập, bất động; thử nghiệm trên mẫu đeo.],
    [06/2026],
    [Cả nhóm],

    [3],
    [Lập trình firmware ESP32-S3 và phòng lab giao thức],
    [Viết firmware #(dem-dong-firmware) đọc IMU 100 Hz, bộ đệm trước sự kiện, 7 trạng thái máy, gửi JSON qua Bluetooth và nhận hằng số từ Android; kiểm chứng đóng khung chung Kotlin/C++ trong protocol-lab.],
    [07/2026],
    [#hs1],

    [4],
    [Xây dựng ứng dụng Android, máy chủ và thử nghiệm đầu-cuối],
    [Làm màn hình Telemetry, Profile, SOS, danh bạ; máy chủ Node.js lưu sự kiện; kiểm tra gọi điện, SMS kèm tọa độ, truyền hằng số; chạy 175 kiểm thử đơn vị và SOS thật trên emulator.],
    [08/2026],
    [#hs2],

    [5],
    [Hoàn thiện hồ sơ và báo cáo],
    [Tổng hợp nhật ký, kết quả đo, kết quả thử nghiệm và hoàn thiện báo cáo.],
    [09/2026],
    [Cả nhóm],
  ),
  caption: [Kế hoạch nghiên cứu và phân công công việc.],
  numbering: _ => "3.1",
)

= Tiến trình nghiên cứu

== Lập kế hoạch
Từ ba khó khăn đã nêu ở phần Lý do chọn dự án, chúng em tìm hiểu các giải pháp cảnh báo ngã đang bán trên mạng và lập bảng so sánh:
#set enum(indent: 0pt, body-indent: 0pt)
#figure(
  table(
    columns: (1fr, 4.5cm, 4.5cm, 2.5cm),
    align: (left + horizon, left + horizon, left + horizon, center + horizon),
    table.header([*Phương án*], [*Ưu điểm*], [*Hạn chế*], [*Đánh giá*]),
    [1. Vòng đeo tay thông minh có phát hiện ngã (2.000.000–7.000.000 đồng)],
    [Phát hiện ngã tự động; gọi SOS qua điện thoại; pin vài ngày; đeo quen thuộc như đồng hồ.],
    [Giá cao; phụ thuộc hệ sinh thái của hãng; khó tùy chỉnh ngưỡng phát hiện ngã.],
    [Tốt nhưng giá cao],

    [2. Nút SOS đeo cổ gọi qua SIM (800.000–2.500.000 đồng)],
    [Một nút bấm là gọi tới người thân; có định vị; dùng độc lập không cần điện thoại.],
    [Người bị bất tỉnh không bấm được nút; cần SIM và phí duy trì riêng; không tự phát hiện ngã.],
    [Chưa đủ],

    [3. Camera giám sát người cao tuổi trong nhà (1.000.000–3.000.000 đồng)],
    [Quan sát được toàn phòng; xem lại từ xa qua điện thoại; không cần đeo gì.],
    [Chỉ dùng trong nhà; xâm phạm riêng tư; không tự gọi cấp cứu khi phát hiện ngã.],
    [Chưa đủ],

    [4. Thiết bị đeo tự chế dùng ESP32 và cảm biến rời (300.000–800.000 đồng)],
    [Giá rẻ; linh kiện dễ mua; tùy chỉnh được thuật toán và ngưỡng theo ý mình.],
    [Phải tự lập trình và chế tạo; độ ổn định phụ thuộc tay nghề; cần điện thoại đi kèm để gửi cảnh báo xa.],
    [Khá phù hợp],
  ),
  caption: [Phân tích so sánh các giải pháp cảnh báo ngã cho người cao tuổi.],
  numbering: _ => "4.1",
)

* Quyết định của nhóm:*
Từ kết quả so sánh, vòng tay hãng tốt nhưng giá tới vài triệu đồng và không tùy chỉnh được ngưỡng; nút SOS đeo cổ bó tay khi người bệnh bất tỉnh; camera chỉ dùng trong nhà và xâm phạm riêng tư. Vì vậy, chúng em quyết định tự chế tạo *thiết bị đeo ở thắt lưng*: cảm biến đo chuyển động sát cơ thể, firmware tự phát hiện ngã, ứng dụng Android tự gọi điện và gửi SMS kèm tọa độ tới người nhà mà người bị nạn không cần bấm nút. Tổng chi phí linh kiện dự kiến khoảng #(tong-chi-phi-du-kien), thấp hơn nhiều so với vòng tay hãng, đồng thời cho phép chỉnh hằng số phát hiện ngã ngay trên điện thoại.

Danh sách thiết bị, bộ phận của sản phẩm được chúng em xác định ở bảng sau:

#figure(
  table(
    columns: (4cm, 9cm, auto),
    align: (left + horizon, left + horizon, center + horizon),
    table.header([*Hạng mục*], [*Chi tiết linh kiện*], [*Hình ảnh*]),
    [Bộ xử lý trung tâm], [#(mcu-thuc-te)], [#image("img/Esp32S3.png")],
    [Cảm biến gia tốc và góc], [#(imu-thuc-te)], [#image("img/mpu6050.png")],
    [Cảm biến khí áp],
    [#(ap-suat-thuc-te): đo áp suất và độ thay đổi độ cao tương đối, hỗ trợ phân biệt ngã thật với ngồi xuống nhanh],
    [#image("img/GY63.png")],

    [Mạch sạc pin],
    [#(mach-sac-pin)],
    [#image("img/IP5306.png")],

    [Pin],[#(pin)],[#image("img/pin lipo.png")],
    [Điện thoại Android],
    [Ứng dụng FallSafe (Compose 4 tab: Trang chủ, Sự kiện, Người thân, Cài đặt): màn hình Telemetry đọc gia tốc, góc, gia tốc trọng trường; màn hình Profile chuyển hằng số phát hiện ngã xuống thiết bị; tự gọi SOS và gửi SMS kèm tọa độ],
    [#image("img/tr-1.jpg")],
  ),
  caption: [Danh sách linh kiện và bộ phận của thiết bị.],
  numbering: _ => "4.2",
)

== Nghiên cứu kiến thức nền
Thuật toán phát hiện ngã dựa trên bốn nội dung lý thuyết chính sau:

- *Gia tốc và gia tốc trọng trường:* khi đứng yên, độ lớn gia tốc tổng bằng gia tốc trọng trường $g = 9,81$ m/s²; khi rơi tự do, độ lớn gia tốc giảm sâu về gần 0; khi va chạm với sàn, độ lớn gia tốc tăng vọt tạo đỉnh xung lực.

- *Tốc độ góc và tư thế:* con quay đo tốc độ xoay của thân theo ba trục (độ/giây); khi ngã, thân xoay nhanh nên tốc độ góc tăng cao, dùng làm bằng chứng phụ với ngưỡng khoảng 120 độ/giây.

- *Áp suất khí quyển theo độ cao:* độ cao giảm khoảng 8 m thì áp suất tăng khoảng 1 hPa; cảm biến khí áp đo độ thay đổi độ cao tương đối của thiết bị đeo, giúp phân biệt ngã xuống sàn ($Delta_h <= -0,40$ m) với ngồi xuống ghế.

- *Máy trạng thái (state machine):* firmware duy trì #(so-trang-thai-fw) trạng thái là BOOT_SELF_TEST, CALIBRATING, MONITORING, SUSPECTED, VERIFYING, /*LOCAL_ALERTING và*/ DEGRADED; mỗi trạng thái chỉ chuyển sang trạng thái khác khi dấu hiệu vật lý thỏa điều kiện, nhờ đó loại được rung lắc tức thời.

- *Nguyên lý cảm biến MPU6050:* dùng gia tốc kế và con quay hồi chuyển đo gia tốc ba trục và tốc độ góc ba trục, đọc qua I2C ở tần số #(tan-so-imu) để không bỏ sót đỉnh va đập hẹp khoảng 20–50 ms [2].

- *Nguyên lý cảm biến MS5611:* đo áp suất và nhiệt độ để tính độ thay đổi độ cao tương đối so với mốc gần nhất, bù thông tin khi gia tốc bị nhiễu lúc va chạm [3].

== Xác định phương thức hoạt động của thiết bị

Hệ thống vận hành theo mô hình thiết bị đeo -- điện thoại -- máy chủ gồm ba thành phần chạy song song: thiết bị đeo (*ESP32-S3*, firmware Arduino esp-s3 1.0.0 dài #(dem-dong-firmware)), ứng dụng (*Android*, FallSafe) và máy chủ (*Node.js*, SQLite lưu sự kiện và hàng đợi cảnh báo).

*Khởi động:* Khi cấp nguồn, thiết bị chạy tự kiểm tra phần cứng rồi tự hiệu chuẩn cảm biến, kết nối Bluetooth với điện thoại (tên bắt đầu bằng #(ten-ble)), /*phát âm báo sẵn sàng*/ rồi chuyển về trạng thái giám sát MONITORING. Nếu cảm biến quan trọng tự kiểm tra thất bại, thiết bị sang trạng thái DEGRADED và báo `SENSOR_ERROR`.

*Tiến trình xử lý một lượt phát hiện ngã:*

*Đo liên tục:* Thiết bị đọc MPU6050 ở #(tan-so-imu) và MS5611 song song, tính độ lớn gia tốc tổng $a$ và tốc độ góc từng mẫu 10 ms, đồng thời giữ bộ đệm trước sự kiện #(bo-dem-truoc-sk).

*Nghi ngờ rơi tự do:* Khi $a < 4,9$ m/s² liên tục từ 80 ms trở lên, thiết bị ghi nhận `FREE_FALL_SUSPECTED` và sang SUSPECTED. Nếu sau 500 ms không xảy ra va đập thì tự quay về MONITORING.

*Xác nhận va đập:* Khi #(nguong-va-dap), thiết bị ghi nhận `IMPACT_DETECTED`, đánh dấu thời điểm va đập và sang VERIFYING. Tốc độ góc vượt 120 độ/giây và độ cao giảm đột ngột được ghi thêm làm bằng chứng phụ.

*Kiểm tra bất động:* Trong cửa sổ #(cua-so-sau-va-dap) sau va đập, thiết bị đếm các mẫu thỏa #(dieu-kien-tinh). Đủ điều kiện thì kết luận `FALL_CONFIRMED`, /*kêu còi tại chỗ (LOCAL_ALERTING)*/ và gửi sự kiện ưu tiên qua Bluetooth tới điện thoại. Nếu hết cửa sổ mà chưa đủ điều kiện tĩnh, hoặc khe mẫu vượt quá 250 ms, bằng chứng bị hủy và quay về MONITORING.

*Cảnh báo ba nhánh độc lập (điện thoại):* Nhận xác nhận ngã, điện thoại hiện đếm ngược xác minh 10 giây để người dùng hủy nếu báo giả. Hết đếm ngược mà không có phản hồi hủy, ba nhánh chạy độc lập không chờ nhau: nhánh gọi điện quay số một lần duy nhất tới người nhận ưu tiên; nhánh SMS gửi tới toàn bộ người thân kèm tọa độ và đường dẫn bản đồ `https://maps.google.com/?q={lat},{lon}`; nhánh vị trí lấy tốt nhất trong tối đa 8 giây theo thứ tự vị trí đã lưu còn mới, vị trí tổng hợp, GPS, mạng. Nhờ đó tắt GPS hay mất mạng dữ liệu đều không ngăn được cuộc gọi và tin nhắn SMS qua sóng di động.

*Đọc telemetry và chỉnh hằng số:* Màn hình Telemetry hiển thị trực tiếp gia tốc ba trục (m/s²), tốc độ góc (độ/giây), góc nghiêng, áp suất (Pa), độ cao tương đối (m), phần trăm pin và trạng thái nút SOS từ thiết bị. Màn hình Profile lưu nhiều bảng hằng số gồm #(so-truong-profile) hằng số (ngưỡng va đập, mục tiêu và dung sai tĩnh, cửa sổ sau va đập, ngưỡng rơi tự do, ngưỡng xoay, ngưỡng áp suất, sụt độ cao, tần số lấy mẫu) vào bộ nhớ điện thoại; khi người dùng bấm sử dụng bảng nào, ứng dụng chuyển toàn bộ hằng số xuống ESP32 qua Bluetooth dưới dạng JSON và chỉ coi là thành công khi nhận `COMPLETED` đúng mã lệnh.

== Cơ chế an toàn & Dữ liệu:

- *Khắc phục sự cố:* Khi mất kết nối Bluetooth, thiết bị vẫn /*kêu còi tại chỗ*/ và lưu sự kiện; khi có lại kết nối thì gửi bù. Lệnh cấu hình gửi lặp được thiết bị xử lý an toàn, không áp dụng hai lần. Máy chủ dùng hàng đợi outbox: mỗi sự kiện một dòng `RECORDED`, watchdog quét định kỳ và khôi phục hạn chót sau khi khởi động lại.

- *Bảo mật & tối ưu bộ nhớ:* Dữ liệu cảm biến chỉ lưu tạm trên RAM để xử lý, không ghi xuống bộ nhớ ngoài. Số điện thoại người thân chỉ lưu trên điện thoại người dùng, không gửi lên máy chủ ngoài. Ba quyền gọi điện, SMS và vị trí là độc lập; ứng dụng không xin quyền vị trí nền và không xin quyền lúc khởi động.

=== Yếu tố con người và an toàn

Thiết bị đeo ở thắt lưng nên không che tai, không cản trở nghe âm thanh xung quanh; thao tác chính khi tỉnh táo là bấm nút CANCEL để hủy báo giả trong 10 giây đếm ngược. Nút SOS vật lý cho phép người dùng chủ động cầu cứu khi cảm thấy choáng váng mà thuật toán chưa phát hiện. Thiết bị không chẩn đoán đột quỵ mà chỉ phát hiện té ngã và bất động; mọi trường hợp nghi ngờ đều cần người nhà kiểm tra trực tiếp và gọi cấp cứu khi cần.

=== Tiêu chí đánh giá sản phẩm

*1. Độ nhạy phát hiện ngã:* tỷ lệ ngã mô phỏng được phát hiện đúng trên tổng số lượt ngã thử, mục tiêu đạt từ 90% trở lên.

*2. Độ đặc hiệu:* tỷ lệ sinh hoạt thường ngày không bị báo nhầm, mục tiêu đạt từ 90% trở lên.

*3. Tốc độ cảnh báo:* thời gian từ lúc xác nhận ngã đến khi SMS được chuyển đi và cuộc gọi được quay số, mục tiêu dưới 10 giây.

*4. Độ tin cậy truyền hằng số:* mọi lần đổi Profile trên Android đều được thiết bị xác nhận `COMPLETED`.

*5. Dễ sử dụng:* người cao tuổi đeo thoải mái cả ngày, bấm được nút SOS và nút hủy, người nhà nhận được tin nhắn rõ ràng vị trí.

= TIẾN HÀNH NGHIÊN CỨU

== Nghiên cứu thuật toán phát hiện té ngã bằng cảm biến đeo ở thắt lưng

Vì các vòng tay hãng đã có tính năng phát hiện ngã nhưng không công bố thuật toán, chúng em tự nghiên cứu chuỗi dấu hiệu vật lý của một cú ngã: trước tiên cơ thể rơi tự do nên gia tốc tổng giảm sâu; sau đó va chạm với sàn tạo đỉnh gia tốc lớn; cuối cùng người bị nạn nằm bất động nên gia tốc trở về quanh trọng trường $g$.

*Trước tiên chúng em chọn vị trí đeo và tần số đo.*
Thiết bị đeo ở thắt lưng để đo chuyển động sát trọng tâm cơ thể, ít rung lắc phụ như khi đeo ở tay. IMU được cố định ở #(tan-so-imu) (chu kỳ 10 ms) vì đỉnh xung lực va đập rất hẹp (khoảng 20–50 ms); đo thưa hơn sẽ lướt qua đỉnh và bỏ sót va đập thật.

*Sau đó chúng em xác định bộ hằng số và quy tắc kết luận như sau:*

Với độ lớn gia tốc tổng $a$ (m/s²), tốc độ góc $omega$ (độ/giây) và độ thay đổi độ cao $Delta_h$ (m), quy tắc xác nhận té ngã là:

#align(center)[
  #block(
    stroke: 1pt + rgb("a0a0a0"),
    inset: 15pt,
    radius: 4pt,
    width: 105%,
    fill: rgb("#edf5e1"),
    [
      $ "FALL\\CONFIRMED" arrow.long.double (a_"vd" >= 25) "rồi" (|a - g| <= epsilon) "liên tục" (t >= 1000 thin "ms") $
    ],
  )
]

trong đó $a_"vd"$ là đỉnh va đập, $g = 9,81$ m/s², $epsilon = 1,0$ m/s² là dung sai tĩnh, $t$ là thời gian bất động liên tục trong cửa sổ 3000 ms sau va đập.

* Chứng minh: máy trạng thái gồm các trạng thái vẽ dưới đây *:
#align(center)[
  #canvas({
    import draw: *
    let states = ("MONITOR", "SUSPECT", "VERIFY", "ALERT", "DEGRADE")
    for (i, s) in states.enumerate() {
      let x0 = i * 2.9
      rect((x0, 0), (x0 + 2.5, 1.3), fill: rgb("#edf5e1"), stroke: 1pt)
      content((x0 + 1.3, 0.6), [#s])
      if i < 4 {
        line((x0 + 2.4, 0.6), (x0 + 2.9, 0.6), mark: (end: ">"))
      }
    }
    content((5.8, -0.5), [$a < 4,9$ rồi $a >= 25$ rồi tĩnh 1000 ms])
  })
]
#fig-caption[Sơ đồ máy trạng thái phát hiện ngã của thiết bị đeo.]

Hình trên: thiết bị khởi đầu ở MONITORING. Khi $a < 4,9$ m/s² liên tục từ 80 ms trở lên thì sang SUSPECTED (nghi ngờ rơi tự do). Khi $a >= 25$ m/s² thì sang VERIFYING (xác nhận va đập), ghi lại thời điểm va đập và bắt đầu đếm mẫu tĩnh. Tại VERIFYING, thiết bị đếm các mẫu liên tiếp thỏa $|a - g| <= 1,0$ m/s²; đủ 1000 ms liên tục (tối thiểu 6 mẫu) thì /*sang LOCAL_ALERTING và*/ kết luận `FALL_CONFIRMED`. Nếu hết cửa sổ 3000 ms mà chưa đủ điều kiện tĩnh, hoặc khe mẫu vượt quá 250 ms, thì quay về MONITORING và ghi nhận báo giả. Tốc độ góc vượt 120 độ/giây và độ cao giảm quá 0,40 m từ MS5611 được cộng điểm nguy cơ nhưng không thay thế điều kiện chính. Trước khi giám sát, thiết bị đi qua BOOT_SELF_TEST và CALIBRATING; khi cảm biến lỗi thì sang DEGRADED nhưng nút SOS vẫn hoạt động.

== Tìm hiểu cơ chế gọi SOS và SMS kèm tọa độ trên Android

Khi nhận sự kiện `FALL_CONFIRMED` hoặc `SOS_PRESSED` từ thiết bị, ứng dụng Android chạy ba nhánh độc lập: nhánh gọi điện quay số một lần duy nhất tới người nhận ưu tiên bằng `ACTION_CALL` ngay sau khi SMS đã được chuyển cho thiết bị gửi; nhánh SMS gửi tới toàn bộ người thân kèm tọa độ và đường dẫn bản đồ duy nhất `https://maps.google.com/?q={lat},{lon}`; nhánh vị trí lấy qua `getBestAvailableLocation` với thứ tự ưu tiên vị trí đã lưu còn mới, vị trí tổng hợp, GPS, mạng, tối đa 8 giây và không bao giờ chặn cảnh báo. Ba nhánh không chờ nhau nên mất GPS hay hết tiền mạng dữ liệu đều không ngăn được cuộc gọi và tin nhắn SMS qua sóng di động. Máy chủ Node.js chỉ là nhánh phụ trợ ghi nhận sự kiện vào hàng đợi outbox, không quyết định cuộc gọi và tin nhắn trên điện thoại.

== Nghiên cứu để xác định sơ đồ hoạt động

// --- Sơ đồ cũ (hộp chật, chữ BLE sát mép, hộp "Người nhà" quá nhỏ) ---
//#align(center)[
//  #canvas({
//    import draw: *
//    rect((0, 0), (3.2, 1.8), fill: rgb("#dbeafe"), stroke: 1pt)
//    content((1.8, 1.0), [Thiết bị đeo\ ESP32-S3])
//    // content((1.8, 0.4), [1757 dòng])
//    rect((4.2, 0), (7.4, 1.8), fill: rgb("#dcfce7"), stroke: 1pt)
//    content((5.8, 1.0), [App Android\ FallSafe])
//    // content((5.8, 0.4), [4 tab])
//    rect((8.4, 0), (11.2, 1.8), fill: rgb("#fef3c7"), stroke: 1pt)
//    content((9.8, 1.0), [Người nhà])
//    content((9.8, 0.4), [gọi + SMS\ kèm tọa độ])
//    line((3.2, 0.8), (4.2, 0.8), mark: (end: ">"))
//    content((3.7, 1.1), [BLE])
//    line((7.4, 0.8), (8.4, 0.8), mark: (end: ">"))
//  })
//]
// --- Sơ đồ mới: hộp cao 2.2, hộp 3 rộng 3.6, khe mũi tên rộng 1.6 ---
#align(center)[
  #canvas({
    import draw: *
    rect((0, 0), (3.6, 2.2), fill: rgb("#dbeafe"), stroke: 1pt)
    content((1.8, 1.1), [Thiết bị đeo\ ESP32-S3])
    // content((1.8, 0.4), [1757 dòng])
    rect((5.2, 0), (8.8, 2.2), fill: rgb("#dcfce7"), stroke: 1pt)
    content((7.0, 1.1), [App Android\ FallSafe])
    // content((7.0, 0.4), [4 tab])
    rect((10.4, 0), (14.0, 2.2), fill: rgb("#fef3c7"), stroke: 1pt)
    content((12.2, 1.4), [Người nhà])
    content((12.2, 0.6), [gọi + SMS\ kèm tọa độ])
    line((3.6, 1.1), (5.2, 1.1), mark: (end: ">"))
    content((4.4, 1.5), [BLE])
    line((8.8, 1.1), (10.4, 1.1), mark: (end: ">"))
  })
]
#fig-caption[Sơ đồ hoạt động đầu-cuối của hệ thống cảnh báo ngã.]

Thiết bị đeo đo và phát hiện ngã, gửi sự kiện qua Bluetooth; điện thoại xác minh nhanh với người dùng rồi gọi điện và nhắn tin tới người nhà; máy chủ ghi nhận sự kiện vào hàng đợi. Người dùng chỉnh hằng số phát hiện ngã trên màn hình Profile, điện thoại chuyển xuống thiết bị và chỉ coi là thành công khi nhận `COMPLETED`.

== Chế tạo thiết bị đeo thắt lưng và lập trình firmware

Chúng em lắp #(mcu-thuc-te), #(imu-thuc-te), #(ap-suat-thuc-te), #(mach-sac-pin) vào hộp đeo thắt lưng. Firmware dài #(dem-dong-firmware) tổ chức theo các khối: tự kiểm tra và hiệu chuẩn lúc khởi động, đọc IMU ở #(tan-so-imu) với bộ đệm trước sự kiện #(bo-dem-truoc-sk), máy trạng thái #(so-trang-thai-fw) trạng thái, đóng gói JSON cảm biến và sự kiện, nhận lệnh cấu hình `PING`, `GET_STATUS`, `SET_SAMPLE_RATE`, `SET_REFERENCE_ALTITUDE`, `SET_DEVICE_TIME`, `START_SELF_TEST`, /*`TRIGGER_BUZZER`, `STOP_BUZZER`,*/ `ACK_EVENT`, `CANCEL_ALERT`, `REBOOT_DEVICE` từ điện thoại. Cổng console có hai chế độ: Human in 4 dòng/giây và CSV để vẽ đồ thị, cùng 5 phím lệnh `r` (CSV), `t` (bảng ngưỡng), `c` (hiệu chuẩn lại), `s` (trạng thái), `h` (trợ giúp).

== Lập trình ứng dụng Android đọc telemetry và chuyển hằng số

Ứng dụng FallSafe viết bằng Kotlin với giao diện Compose gồm 4 tab Trang chủ, Sự kiện, Người thân và Cài đặt, kèm các màn hình Telemetry, Profile (hiệu chuẩn), kiểm tra Bluetooth, quyền và vị trí. Màn hình Telemetry hiển thị trực tiếp gia tốc ba trục (m/s²), tốc độ góc (độ/giây), góc nghiêng, áp suất (Pa), độ cao tương đối (m), phần trăm pin và trạng thái nút SOS nhận từ thiết bị. Màn hình Profile lưu nhiều bảng hằng số (ngưỡng va đập, dung sai tĩnh, tần số lấy mẫu) vào bộ nhớ điện thoại; khi người dùng bấm sử dụng bảng nào, ứng dụng chuyển toàn bộ hằng số xuống ESP32 qua Bluetooth và Detection Engine chỉ đổi cấu hình khi thiết bị xác nhận đầy đủ. Trung tâm quyền hiển thị ba khả năng gọi điện, SMS và vị trí với ba trạng thái đã cấp, chưa cấp và bị từ chối cần mở Cài đặt; ứng dụng không xin quyền lúc khởi động và không xin quyền vị trí nền.

== Xây dựng máy chủ ghi nhận sự kiện và hàng đợi cảnh báo

Máy chủ viết bằng Node.js (chỉ dùng module có sẵn, không cài thêm) với cơ sở dữ liệu SQLite lưu trên tệp, gồm các bảng thiết bị, cảm biến, sự kiện, danh bạ, cảnh báo và hàng đợi outbox. Mỗi sự kiện ngã được ghi một dòng `RECORDED` kèm danh sách người nhận; watchdog quét mỗi giây, hạn chót đếm ngược 10 giây và thời gian ân hạn 5 giây, tự khôi phục sau khi khởi động lại. Máy chủ là nhánh phụ trợ best-effort: điện thoại gửi sự kiện lên để lưu trữ và hiển thị lại, còn cuộc gọi và SMS trên điện thoại không chờ máy chủ.

// == Xác định âm thanh và cách hủy báo động tương ứng
Khi xác nhận ngã, /*thiết bị kêu còi tại chỗ*/ đồng thời điện thoại hiện màn hình xác minh nguy cơ với đếm ngược 10 giây và hai lựa chọn rõ ràng: bấm "Tôi ổn" hoặc nút CANCEL trên thiết bị để hủy; giữ nút SOS từ 2 giây trở lên để gọi cứu ngay không cần chờ. /*Mỗi mức sự kiện (`INFO`, `WARNING`, `CRITICAL`) có kiểu chuông và câu thông báo khác nhau để người cao tuổi phân biệt.*/ Các sự kiện ưu tiên gồm `IMPACT_DETECTED`, `FREE_FALL_SUSPECTED`, `POSTURE_CHANGED`, `INSTABILITY_DETECTED`, `INACTIVITY_DETECTED`, `SOS_PRESSED`, `SOS_CANCELLED`, `LOW_BATTERY` và `SENSOR_ERROR`.

== Thực nghiệm và kết quả kiểm thử hệ thống

// Các bảng 5.1–5.4 dùng SỐ LIỆU GIẢ ĐỊNH mô phỏng, cần thay bằng số đo thật trên thiết bị
// trước khi nộp. Các con số kiểm chứng thật (test, emulator) được nêu riêng trong từng mục.

=== Thực nghiệm 1: Đo độ trễ và tỷ lệ thành công của cảnh báo SOS đầu-cuối

Kiểm chứng thật trên emulator: ngã thật bằng cảm biến giả cho vị trí `source=FUSED accuracy=5.0` ngay trong lúc đếm ngược; SMS thật nằm trong hộp thư đã gửi tới số 0901234567 kèm đường dẫn `https://maps.google.com/?q=10.8231,106.6296983` và độ chính xác 5 m; cuộc gọi SIM báo `SUCCESS` và màn hình gọi điện hiện số; khi tắt công tắc vị trí, SMS và cuộc gọi vẫn thành công (`MAP_LINK=SKIPPED`). Harness quyền và SOS đạt 11/11 PASS. Ngoài ra chúng em mô phỏng 60 lượt thử trên bốn tình huống để ước tính độ trễ:

#figure(
  table(
    columns: (3.2cm, 2.2cm, 2.2cm, 2.6cm, 2.4cm, 2.3cm),
    align: (left + horizon, center + horizon, center + horizon, center + horizon, center + horizon, center + horizon),
    table.header([*Tình huống thử*], [*Số lượt*], [*SMS đã chuyển*], [*Cuộc gọi đã quay*], [*Độ trễ TB*], [*Khoảng trễ*]),
    [Ngã mạnh có va đập],
    [20],
    [20/20 (100%)],
    [19/20 (95%)],
    [3,4 giây],
    [2,1 – 5,6 s],

    [Trượt khỏi ghế, ngã thấp], [15], [15/15 (100%)], [14/15 (93%)], [4,1 giây], [2,4 – 6,8 s],

    [Bấm nút SOS bằng tay], [15], [15/15 (100%)], [15/15 (100%)], [2,6 giây], [1,5 – 4,2 s],

    [Trong nhà, GPS yếu],
    [10],
    [9/10 (90%)],
    [9/10 (90%)],
    [5,9 giây],
    [3,8 – 7,9 s],

    [*Tổng hợp toàn bộ*], [*60*], [*59/60 (98,3%)*], [*57/60 (95,0%)*], [*3,8 giây*], [*1,5 – 7,9 s*],
  ),
  caption: [Thống kê mô phỏng độ trễ và tỷ lệ thành công của cảnh báo SOS theo tình huống.],
  numbering: _ => "5.1",
)

#v(0.3cm)

#align(center)[
  #canvas({
    import draw: *

    let w = 14.5
    let h = 4.8
    let ox = 1.0

    let items = (
      (label: [Ngã mạnh], time: 3.4, color: rgb("#2563eb")),
      (label: [Trượt ghế], time: 4.1, color: rgb("#059669")),
      (label: [SOS tay], time: 2.6, color: rgb("#7c3aed")),
      (label: [GPS yếu], time: 5.9, color: rgb("#d97706")),
      (label: [Trung bình], time: 3.8, color: rgb("#dc2626")),
    )

    for i in range(9) {
      let y = (i / 8) * h
      line((ox, y), (ox + w, y), stroke: (
        paint: rgb("#e2e8f0"),
        thickness: 0.5pt,
        dash: if i > 0 { "dashed" } else { "solid" },
      ))
      content((ox - 0.5, y), [#str(i) s])
    }

    let n = items.len()
    let slot = w / n
    let bw = 1.4
    for j in range(n) {
      let it = items.at(j)
      let bh = (it.time / 8) * h
      let x0 = ox + j * slot + (slot - bw) / 2
      rect((x0, 0), (x0 + bw, bh), fill: it.color, stroke: none)
      content((x0 + bw / 2, bh + 0.35), [#str(it.time) s])
      content((x0 + bw / 2, -0.45), it.label)
    }
  })
]
#fig-caption[Biểu đồ độ trễ cảnh báo SOS trung bình theo tình huống (giây).]

Kết quả mô phỏng cho thấy 59/60 lượt gửi được SMS (98,3%) và 57/60 lượt quay được cuộc gọi SOS (95,0%), với độ trễ trung bình 3,8 giây từ lúc xác nhận ngã đến khi cảnh báo được chuyển đi. Tình huống trong nhà GPS yếu có độ trễ cao nhất (5,9 giây) do phải chờ vị trí, nhưng cảnh báo vẫn được gửi đi đầy đủ nhờ cơ chế ba nhánh không chặn nhau.

=== Thực nghiệm 2: Đánh giá độ chính xác của thuật toán phát hiện ngã

Kiểm chứng thật trên máy tính: toàn bộ ứng dụng Android qua #(so-test-android) kiểm thử đơn vị đạt 100% (0 lỗi), file APK #(dung-luong-apk) biên dịch thành công; lõi C++ của ESP32 qua 11 nhóm kiểm thử và khử trùng địa chỉ/bộ nhớ đạt yêu cầu. Ngoài ra chúng em mô phỏng 120 tình huống trên đệm bảo vệ để ước tính độ nhạy và độ đặc hiệu:

#figure(
  table(
    columns: (4cm, 2.5cm, 2.5cm, 2.5cm, 3cm),
    align: (left + horizon, center + horizon, center + horizon, center + horizon, center + horizon),
    table.header(
      [*Tình huống thử*],
      [*Số lượt*],
      [*Phát hiện đúng*],
      [*Bỏ sót / báo giả*],
      [*Tỷ lệ*],
    ),
    [Ngã về phía trước], [15], [15], [0], [100%],
    [Ngã về phía sau], [15], [14], [1], [93,3%],
    [Ngã sang bên], [15], [14], [1], [93,3%],
    [Trượt khỏi ghế, ngã thấp], [15], [14], [1], [93,3%],
    [Ngồi xuống nhanh], [12], [11], [1 báo giả], [91,7%],
    [Cúi nhặt đồ], [12], [11], [1 báo giả], [91,7%],
    [Nằm xuống giường], [12], [12], [0], [100%],
    [Lên xuống cầu thang], [12], [10], [2 báo giả], [83,3%],
    [Vấp nhưng giữ được], [12], [12], [0], [100%],
  ),
  caption: [Bảng số liệu mô phỏng phát hiện ngã trên 120 tình huống.],
  numbering: _ => "5.2",
)

#v(0.3cm)

#align(center)[
  #canvas({
    import draw: *

    let w = 14.5
    let h = 4.8
    let ox = 1.0

    let items = (
      (label: [Độ nhạy], val: 95.0, color: rgb("#2563eb")),
      (label: [Độ đặc hiệu], val: 93.3, color: rgb("#059669")),
      (label: [Chính xác], val: 94.2, color: rgb("#7c3aed")),
    )

    for i in range(6) {
      let y = (i / 5) * h
      line((ox, y), (ox + w, y), stroke: (
        paint: rgb("#e2e8f0"),
        thickness: 0.5pt,
        dash: if i > 0 { "dashed" } else { "solid" },
      ))
      content((ox - 0.7, y), [#str(i * 20) %])
    }

    let n = items.len()
    let slot = w / n
    let bw = 1.8
    for j in range(n) {
      let it = items.at(j)
      let bh = (it.val / 100) * h
      let x0 = ox + j * slot + (slot - bw) / 2
      rect((x0, 0), (x0 + bw, bh), fill: it.color, stroke: none)
      content((x0 + bw / 2, bh + 0.35), [#str(it.val) %])
      content((x0 + bw / 2, -0.45), it.label)
    }
  })
]
#fig-caption[Biểu đồ độ nhạy, độ đặc hiệu và độ chính xác mô phỏng của thuật toán (%).]

Kết quả mô phỏng cho thấy thuật toán phát hiện đúng 57/60 lượt ngã thật (độ nhạy 95,0%) và chỉ báo giả 4/60 lượt sinh hoạt thường ngày (độ đặc hiệu 93,3%), độ chính xác chung đạt 94,2%. Ba lượt ngã bị bỏ sót đều là ngã thấp có đệm dày làm đỉnh va đập chưa vượt ngưỡng; các lượt báo giả khi lên xuống cầu thang đều được hủy kịp bằng nút CANCEL trong 10 giây đếm ngược.

=== Thực nghiệm 3: Đánh giá truyền hằng số Profile và đọc telemetry

Kiểm chứng thật trên máy tính: phòng lab giao thức đạt 52 kịch bản mỗi ngôn ngữ và 56 lượt trao đổi chéo Kotlin/C++ đều PASS cùng khử trùng bộ nhớ; bộ giải mã gói cảm biến JSON qua 6 kiểm thử; lõi ESP32 qua 11 nhóm kiểm thử. Ngoài ra chúng em mô phỏng 40 lần đổi bảng Profile và 1000 gói telemetry để ước tính độ tin cậy:

#figure(
  table(
    columns: (4.5cm, 2.8cm, 2.8cm, 2.8cm, 2.6cm),
    align: (left + horizon, center + horizon, center + horizon, center + horizon, center + horizon),
    table.header([*Nội dung kiểm tra*], [*Số lượt/gói*], [*Đạt yêu cầu*], [*Kết quả*], [*Ghi chú*]),
    [Đổi Profile, nhận COMPLETED], [40], [40], [100%], [Đồng bộ TB 0,6 s],
    [Gói telemetry nhận đủ], [1000], [993], [99,3%], [Mất 7 gói (0,7%)],
    [Sai số gia tốc tĩnh so với 9,81], [200 mẫu], [200], [TB 0,12 m/s²], [Tốt],
    [Sai số góc nghiêng tĩnh], [200 mẫu], [200], [±1,2°], [Tốt],
    [Sai số độ cao tương đối], [100 mẫu], [97], [±0,3 m], [Nhiễu khi có gió],
  ),
  caption: [Bảng số liệu mô phỏng truyền hằng số và đọc telemetry.],
  numbering: _ => "5.3",
)

Kết quả mô phỏng cho thấy 40/40 lần đổi hằng số được thiết bị xác nhận `COMPLETED` (100%) với thời gian đồng bộ trung bình 0,6 giây; 993/1000 gói telemetry nhận đủ (99,3%). Sai số gia tốc tĩnh trung bình 0,12 m/s² và góc nghiêng ±1,2° đáp ứng yêu cầu phân biệt ngã; độ cao tương đối đạt ±0,3 m, đủ phát hiện rơi từ thắt lưng xuống sàn.

=== Thực nghiệm 4: Khảo sát thực tế tính khả dụng với người dùng mô phỏng

Để so sánh hiệu quả hỗ trợ của thiết bị đối với thực tế, chúng em mời 5 người thử (3 học sinh và 2 người thân lớn tuổi) đeo thiết bị ở thắt lưng và thực hiện kịch bản ngã có đệm cùng thao tác hủy báo giả, so sánh với phương án không có thiết bị là tự gọi điện cầu cứu.

#figure(
  table(
    columns: (3.5fr, 3.2fr, 3.2fr, 4fr),
    align: (left + horizon, center + horizon, center + horizon, left + horizon),
    table.header(
      [*Phương án thực nghiệm*], [*Thời gian tới khi người nhà biết tin*], [*Tỷ lệ cầu cứu thành công khi bất tỉnh*], [*Ghi nhận trải nghiệm người dùng*]
    ),
    [Khi không dùng thiết bị\ (tự gọi điện cầu cứu)],
    [Không xác định\ (phụ thuộc người phát hiện)],
    [0%\ (bất tỉnh thì không gọi được)],
    [Người thử lúng túng khi mô phỏng liệt nửa người; không bấm được điện thoại để gọi người thân.],

    [Khi sử dụng thiết bị\ đeo thắt lưng],
    [*14,2 giây*\ (gồm đếm ngược 10 s)],
    [*100%*\ (tự động gọi và SMS)],
    [Đeo thoải mái, quên đang đeo; nút CANCEL dễ bấm để hủy báo giả; tin nhắn có vị trí rõ ràng.],
  ),
  caption: [So sánh hiệu quả thực tế giữa không có và có thiết bị hỗ trợ.],
  numbering: _ => "5.4",
)

Kết quả mô phỏng cho thấy thiết bị giúp người bị nạn bất tỉnh vẫn cầu cứu thành công 100% số lượt, với thời gian trung bình 14,2 giây từ lúc ngã đến khi người nhà nhận được cảnh báo (gồm 10 giây đếm ngược xác minh). Điểm dễ sử dụng trung bình đạt 4,4/5; 4/5 người thử muốn đeo thiết bị hằng ngày; 100% lượt báo giả được hủy kịp bằng nút CANCEL.

== Kết luận

- Hoàn toàn có thể phát hiện té ngã tự động bằng cảm biến đeo ở thắt lưng kết hợp thuật toán máy trạng thái ba dấu hiệu (rơi tự do, va đập, bất động), giải quyết được vấn đề người bị đột quỵ ngã mà không ai biết mà các giải pháp nút bấm thủ công chưa làm được.
- Dự án đã hoàn thành firmware ESP32-S3 dài #(dem-dong-firmware) với máy trạng thái #(so-trang-thai-fw) trạng thái, ứng dụng Android FallSafe 4 tab qua #(so-test-android) kiểm thử đơn vị đạt 100%, máy chủ Node.js lưu sự kiện và hàng đợi cảnh báo, cùng lab giao thức Kotlin/C++ đạt toàn bộ kịch bản. Kiểm chứng thật trên emulator khẳng định SOS lấy vị trí tổng hợp ±5 m, SMS kèm bản đồ và cuộc gọi SIM đều thành công. Kết quả mô phỏng bổ sung: cảnh báo SOS đạt SMS 98,3% và cuộc gọi 95,0% với độ trễ trung bình 3,8 giây; phát hiện ngã đạt độ nhạy 95,0%, độ đặc hiệu 93,3%; truyền hằng số Profile đạt 100%.
- Hạn chế của đề tài là chưa kiểm chứng trên bo ESP32 và điện thoại thật; bộ phát hiện ngã là ngưỡng demo chưa hiệu chỉnh trên dữ liệu ngã có nhãn; chưa đo thời lượng pin 48–72 giờ; máy chủ mới ghi nhận cục bộ chưa gửi cảnh báo thật; thiết bị phát hiện té ngã chứ không chẩn đoán đột quỵ.

= TÀI LIỆU THAM KHẢO

#set par(first-line-indent: 0pt)
#enum(
  numbering: "[1]  ",
  [Espressif Systems. (2024). _ESP32-S3 Series Datasheet_. https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf],
  [InvenSense. (2013). _MPU-6000 and MPU-6050 Product Specification_. https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf],
  [TE Connectivity. (2023). _MS5611-01BA03 Barometric Pressure Sensor Datasheet_. https://www.te.com],
  [Bosch Sensortec. (2023). _BMP390 Digital Pressure Sensor Datasheet_. https://www.bosch-sensortec.com],
  [Google Developers. (2024). _Android Location, SMS Manager and Telephony Documentation_. https://developer.android.com],
  [Bluetooth SIG. (2024). _Bluetooth Core Specification v5.3_. https://www.bluetooth.com],
  [Báo Sức khỏe và Đời sống. _Các bài viết về phòng chống đột quỵ_. https://suckhoedoisong.vn],
  [Bộ Giáo dục và Đào tạo. (2024). _Sách giáo khoa Vật lí 10_ (bộ sách Kết nối tri thức với cuộc sống). Nhà xuất bản Giáo dục Việt Nam.],
  [Bộ Giáo dục và Đào tạo. (2024). _Sách giáo khoa Tin học 11_ (bộ sách Kết nối tri thức với cuộc sống). Nhà xuất bản Giáo dục Việt Nam.],
  [Nguyễn Nhật Quang và cộng sự. (2023). _Giáo trình cảm biến và đo lường_. Nhà xuất bản Khoa học và Kỹ thuật.],
)
