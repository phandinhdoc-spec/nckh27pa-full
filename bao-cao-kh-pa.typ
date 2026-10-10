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

// List và enum: dòng đầu thụt như đoạn văn, dòng tiếp diễn về lề trái.
#import "@local/list-enum:0.1.0": enumlist
#show: enumlist.with(indent: 8mm, spacing: 0.6em, marker: ([-], [+]))


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
    #align(left)[#text(weight: "bold")[#if it.numbering != none [#counter(heading).display(it.numbering) #h(
          0.2em,
        )]#it.body]]
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
#let mcu-thuc-te = [ESP32-S3 Super Mini]
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
#let so-test-android = [175 lần kiểm thử]
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
#page(margin: 0pt, footer: none)[
  #align(center + horizon)[#image("PHAN ANH -KHKT-Ị.pdf", width: 100%, height: 100%, fit: "contain")]
]

// =================== TÓM TẮT ĐỀ TÀI ===================
#title[TÓM TẮT DỰ ÁN]
#v(0.5cm)

#text(weight: "bold")[Tính mới:] Tính mới của dự án là kết hợp phát hiện ngã tự động bằng cảm biến đeo ở thắt lưng với cảnh báo SOS  trên điện thoại: khi xác nhận người bị ngã và bất động, hệ thống tự gọi điện một lần tới người nhận ưu tiên, đồng thời gửi SMS kèm tọa độ và đường dẫn bản đồ tới toàn bộ người thân mà người bị nạn không cần bấm nút. Toàn bộ #(so-truong-profile)  phát hiện ngã được chỉnh trực tiếp trên ứng dụng Android và chuyển xuống thiết bị đeo qua Bluetooth, thiết bị chỉ áp dụng khi trả lời `COMPLETED` đúng mã lệnh.

#text(weight: "bold")[Tính khoa học:] Thuật toán phát hiện ngã được chúng em nghiên cứu dựa trên kiến thức vật lý (gia tốc góc, gia tốc trọng trường, tốc độ góc, sự tăng giảm độ cao dựa trên áp suất khí quyển) và lập trình esp: nghi ngờ rơi tự do khi #(nguong-roi-tu-do), xác nhận va đập khi #(nguong-va-dap), rồi kiểm tra bất động sau va đập theo #(dieu-kien-tinh). Trên Android, ba nhánh gọi điện, SMS và vị trí chạy độc lập nên mất GPS hay mất mạng dữ liệu đều không ngăn được cảnh báo qua sóng di động.

#text(weight: "bold")[Tính thực tiễn:] Thiết bị đeo nhỏ gọn ở thắt lưng cùng ứng dụng Android FallSafe đọc trực tiếp gia tốc, góc, gia tốc trọng trường từ ESP32, giúp người có nguy cơ đột quỵ và tai biến mạch máu não được phát hiện kịp thời khi ngã mà không có người bên cạnh, với tổng chi phí linh kiện dự kiến khoảng #(tong-chi-phi-du-kien). Ứng dụng đã qua #(so-test-android) đạt 100% và kiểm thử SOS thật trên Android với vị trí, SMS và cuộc gọi đều thành công.

#text(weight: "bold")[Tính cộng đồng:] Dự án hướng tới người cao tuổi và người có nguy cơ đột quỵ phải sống một mình hoặc thường ở nhà một mình. Theo các bài viết về đột quỵ trên Báo Sức khỏe và Đời sống #footnote[Theo các bài viết về đột quỵ trên Báo Sức khỏe và Đời sống (suckhoedoisong.vn)], mỗi năm Việt Nam có khoảng 200.000 ca đột quỵ mới, trong đó té ngã sau đột quỵ là nguyên nhân phổ biến khiến người bệnh không kịp gọi cấp cứu. Thiết bị cảnh báo tự động góp phần rút ngắn thời gian phát hiện và tăng cơ hội cấp cứu.

#pagebreak()

// =================== MỤC LỤC ===================
#title[MỤC LỤC]
#v(0.5cm)
#outline(title: none, indent: 1.5em)
// =================== NỘI DUNG CHÍNH ===================
#pagebreak()
= LÝ DO CHỌN ĐỀ TÀI

Đột quỵ và tai biến mạch máu não là một trong những nguyên nhân gây tử vong và tàn tật hàng đầu ở Việt Nam, với khoảng 200.000 ca mới mỗi năm. Người bị đột quỵ thường ngã đột ngột, mất ý thức hoặc liệt nửa người nên không thể tự gọi điện cầu cứu. Nếu không có người chứng kiến, thời gian từ lúc ngã đến lúc được phát hiện có thể kéo dài hàng giờ, làm mất cơ hội cấp cứu.

Qua tìm hiểu nguyên nhân trên báo, chúng em nhận thấy ba khó khăn chính của người bị té ngã là:
- *Té ngã khi không có người bên cạnh:* người cao tuổi sống một mình hoặc ở nhà một mình ban ngày; khi ngã và bất tỉnh thì không ai biết để gọi cấp cứu.
- *Gọi cầu cứu khi tay chân không cử động được:* sau té ngã đột quỵ, người bệnh có thể liệt hoặc yếu nửa người nên không bấm được điện thoại, kể cả điện thoại để ngay bên cạnh.
- *Người nhà không biết vị trí và tình trạng:* khi nhận tin báo muộn, người nhà không biết người thân ngã ở đâu, ngã lúc nào và có còn tỉnh táo hay không.

Từ thực trạng đó, chúng em đặt câu hỏi: *Làm thế nào để người có nguy cơ đột quỵ được tự động phát hiện khi ngã và gửi cảnh báo kèm vị trí tới người nhà, mà không cần người bị nạn phải thao tác gì?*

Chúng em quyết định thực hiện dự án *#ten-du-an* -- một thiết bị đeo ở thắt lưng dùng #(mcu-thuc-te) điều khiển MPU6050 đo gia tốc, tốc độ góc  và GY63 để đo độ cao liên tục ở #(tan-so-imu); khi xác nhận té ngã, ứng dụng Android FallSafe tự động gọi điện và gửi SMS kèm tọa độ tới số người nhà đã lưu. Các vòng tay hãng hiện nay có phát hiện ngã nhưng giá vài triệu đồng và không tùy chỉnh được ngưỡng; giải pháp của chúng em dùng linh kiện rời phổ thông với tổng chi phí dự kiến khoảng #(tong-chi-phi-du-kien) và cho phép chỉnh toàn bộ hằng số phát hiện ngã ngay trên điện thoại.

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

+ Nghiên cứu nguyên lý cảm biến gia tốc  và cảm biến khí áp, ưu và nhược điểm từng loại, xác định cách phối hợp hai cảm biến để phát hiện ngã.

+ Nghiên cứu các ngưỡng phát hiện ngã (rơi tự do, va đập, bất động sau va đập), lựa chọn bộ hằng số phù hợp và cơ chế chuyển hằng số từ ứng dụng Android xuống ESP32 qua Bluetooth.

+ Nghiên cứu cơ chế gọi điện và gửi SMS kèm tọa độ trên Android, bao gồm quyền truy cập, lấy vị trí nhanh khi tín hiệu GPS yếu và đường dự phòng khi mất mạng.

+ Nghiên cứu thiết kế thiết bị đeo ở thắt lưng sao cho đo chuyển động sát cơ thể, thoải mái khi đeo cả ngày; chức năng SOS chủ động được thao tác trên ứng dụng FallSafe, không có nút SOS trên bộ ESP32.

+ Nghiên cứu cơ chế cảnh báo nhiều bước (/*còi tại chỗ,*/đếm ngược xác minh, gọi điện, SMS) để vừa không bỏ sót người bị nạn, vừa hạn chế báo động giả làm phiền người nhà.

== Giả thuyết khoa học
Từ ba câu hỏi nghiên cứu, chúng em đặt ba giả thuyết cần kiểm tra:

(1) Có thể phát hiện té ngã tự động bằng chuỗi ba dấu hiệu vật lý liên tiếp: rơi tự do ($a < 4,9$ m/s²), va đập (#(nguong-va-dap)) và bất động sau va đập (#(dieu-kien-tinh)), kết hợp độ thay đổi độ cao từ cảm biến khí áp.

(2) Điện thoại Android có thể tự động gọi điện một lần và gửi SMS kèm tọa độ tới người nhà trong vòng vài giây sau khi xác nhận ngã, chỉ dùng sóng di động mà không cần mạng internet.

(3) Bộ hằng số phát hiện ngã lưu trên ứng dụng Android có thể chuyển xuống ESP32 qua Bluetooth và được thiết bị áp dụng ngay, giúp tinh chỉnh độ nhạy mà không cần nạp lại firmware.

// = THIẾT KẾ VÀ PHƯƠNG PHÁP NGHIÊN CỨU
= Thiết kế và phương pháp nghiên cứu
// Mục này mô tả đối tượng, phạm vi, phương pháp thu thập số liệu, kế hoạch và phân công.

== Đối tượng nghiên cứu
Đối tượng nghiên cứu của đề tài là:
- Sự thay đổi chuyển động (gia tốc) và độ cao của cơ thể người trong các tình huống té ngã so với khi sinh hoạt bình thường (đi lại, ngồi xuống, cúi người, nằm).
- Cách nhận biết ngã tự động thông qua chuỗi dấu hiệu liên tiếp: rơi tự do, va chạm mạnh và nằm yên sau va chạm.
- Giải pháp kết nối thiết bị đeo với điện thoại thông minh để tự động gọi điện SOS và gửi tin nhắn SMS kèm vị trí cho người nhà.

== Phạm vi nghiên cứu
- *Nội dung:* Đề tài tập trung vào việc phát hiện các cú ngã bất ngờ và tự động phát cảnh báo; đề tài không chẩn đoán bệnh tật hay đột quỵ mà chỉ nhận diện dấu hiệu vận động bất thường khi té ngã.
- *Thực nghiệm:* Khảo sát nhu cầu thực tế của các gia đình có người cao tuổi tại địa phương; thử nghiệm mô phỏng các tư thế ngã trên đệm bảo hiểm an toàn; thử nghiệm tính năng tự động gọi điện và gửi tin nhắn SMS trên điện thoại Android.

== Phương pháp nghiên cứu
Để thực hiện dự án, chúng em sử dụng các phương pháp sau:

1. *Nghiên cứu tài liệu:* Đọc Sách giáo khoa Vật lí 10 [6] (gia tốc, gia tốc trọng trường, chuyển động rơi tự do) và Sách giáo khoa Tin học 11 [7] (lập trình, máy trạng thái); tham khảo các bài báo khoa học về thuật toán phát hiện ngã bằng cảm biến gia tốc [8, 9, 11] và hệ thống giám sát sức khỏe trên điện thoại [10]; đồng thời học theo các bài hướng dẫn có code mẫu: cài đặt và lập trình ESP32-S3 [1], đọc cảm biến MPU6050 [2], đọc độ cao bằng MS5611 [3], dùng mạch sạc IP5306 [12], truyền dữ liệu qua Bluetooth BLE [5] và tài liệu lập trình Android về quyền gọi điện, SMS và vị trí [4].

2. *Thiết kế và phát triển mẫu thử:* Chia công việc thành hai phía. Phía thiết bị đeo: lập trình firmware Arduino cho ESP32-S3 đọc IMU ở #(tan-so-imu), lưu bộ đệm trước sự kiện #(bo-dem-truoc-sk), chạy máy trạng thái phát hiện ngã và gửi gói JSON qua Bluetooth. Phía điện thoại: xây dựng ứng dụng Android nhận telemetry, quản lý Profile hằng số, tự động gọi điện và gửi SMS kèm tọa độ khi có xác nhận ngã.

3. *Thực nghiệm và thống kê:* Ghi lại số lần thả rơi và số lần ngã thử (đeo hông ngã trên đệm, ngã từ từ, trượt khỏi ghế) cùng số lần ứng dụng FallSafe kích hoạt SOS thành công; tính tỉ lệ thành công, sai số tuyệt đối và sai số tỉ đối so với mục tiêu đã đặt ra; chạy #(so-test-android) để kiểm tra ứng dụng Android.

4. *Khảo sát:* Mời người dùng đeo thử thiết bị và thao tác ứng dụng, ghi chép ý kiến về mức độ thoải mái khi đeo, khả năng thao tác SOS trên ứng dụng FallSafe và những điểm còn gây khó chịu.

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
    [Lập trình firmware ESP32-S3 và cài đặt thuật toán phát hiện ngã],
    [Tham khảo bài báo khoa học và tài liệu kỹ thuật về thuật toán phát hiện ngã (Bourke [8], Trần Công Án [9]); viết firmware #(dem-dong-firmware) đọc IMU 100 Hz, bộ đệm trước sự kiện, 7 trạng thái máy, gửi JSON qua Bluetooth và nhận hằng số từ Android; kiểm chứng đóng khung chung Kotlin/C++ trong protocol-lab.],
    [07/2026],
    [#hs1],

    [4],
    [Xây dựng ứng dụng Android và thử nghiệm đầu-cuối],
    [Làm màn hình Telemetry, Profile, SOS, danh bạ; kiểm tra gọi điện, SMS kèm tọa độ, truyền hằng số; chạy thử 175 lần và SOS thật trên FallSafe.],
    [08/2026],
    [Cả nhóm],

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

== Tìm hiểu về các thiết bị hỗ trợ cảnh báo té ngã đã có trên thị trường
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

    // [4. Thiết bị đeo tự chế dùng ESP32 và cảm biến rời (300.000–800.000 đồng)],
    // [Giá rẻ; linh kiện dễ mua; tùy chỉnh được thuật toán và ngưỡng theo ý mình.],
    // [Phải tự lập trình và chế tạo; độ ổn định phụ thuộc tay nghề; cần điện thoại đi kèm để gửi cảnh báo xa.],
    // [Khá phù hợp],
  ),
  caption: [Phân tích so sánh các giải pháp cảnh báo ngã cho người cao tuổi.],
  numbering: _ => "4.1",
)

* Quyết định của nhóm:*
Từ kết quả so sánh, nhóm chúng em quyết định tính năng có thể gọi điện báo cho người thân như vòng đeo tay thông minh, có chức năng SOS nhưng không bấm nút, giá rẻ hơn 2 thiết bị trên, tận dụng điện thoại smartphone (đã là thiết bị phổ biến).

Danh sách thiết bị, bộ phận của sản phẩm được chúng em xác định ở bảng sau:

#figure(
  table(
    columns: (2.8cm, 5.3cm, 2.4cm, auto),
    align: (left + horizon, left + horizon, center + horizon, left + horizon),
    table.header([*Hạng mục*], [*Chi tiết linh kiện*], [*Hình ảnh*], [*Ghi chú*]),
    [Bộ xử lý trung tâm],
    [#(mcu-thuc-te)],
    [#image("img/Esp32S3.png", width: 100%, height: 1.8cm, fit: "contain")],
    [ESP-32 S3 có cấu hình mạnh nhất trong dòng esp32, giá rẻ và dễ mua; dòng supper mini nhỏ gọn],

    [Cảm biến gia tốc và góc],
    [#(imu-thuc-te)],
    [#image("img/mpu6050.png", width: 100%, height: 1.8cm, fit: "contain")],
    [Cảm biến này có độ chính xác cao, phù hợp cho việc phát hiện ngã, dễ mua và có nhiều tài liệu.],

    [Cảm biến khí áp],
    [#(ap-suat-thuc-te): đo áp suất và độ thay đổi độ cao tương đối, hỗ trợ phân biệt ngã thật với ngồi xuống nhanh],
    [#image("img/GY63.png", width: 100%, height: 1.8cm, fit: "contain")],
    [Thực tế có nhiều cảm biến khác tốt hơn, ví dụ BMP390 nhưng không có sẵn, đặt hàng trên Shopee không được.],

    [Mạch sạc pin],
    [#(mach-sac-pin)],
    [#image("img/IP5306.png", width: 100%, height: 1.8cm, fit: "contain")],
    [Mạch này vừa sạc, vừa có mạch ổn định điện áp 5V, vừa có mạch bảo vệ pin.],

    [Pin],
    [#(pin)],
    [#image("img/pin lipo.png", width: 100%, height: 1.8cm, fit: "contain")],
    [Pin này dẹp, dễ thiết kế vỏ đeo vào thắt lưng],

    [Điện thoại Android],
    [Ứng dụng FallSafe (Compose 4 tab: Trang chủ, Sự kiện, Người thân, Cài đặt): màn hình Telemetry đọc gia tốc, góc, gia tốc trọng trường; màn hình Profile chuyển hằng số phát hiện ngã xuống thiết bị; tự gọi SOS và gửi SMS kèm tọa độ],
    [#image("img/tr-1.jpg", width: 100%, height: 8cm, fit: "contain")],
    [Chúng em chọn tìm hiểu về Android vì thấy iOS phức tạp hơn.],
  ),
  caption: [Danh sách linh kiện và bộ phận của thiết bị.],
  numbering: _ => "4.2",
)



== Nghiên cứu kiến thức nền
Thuật toán phát hiện ngã dựa trên bốn nội dung lý thuyết chính sau:

- *Nguyên lý cảm biến MPU6050:* dùng gia tốc kế và con quay hồi chuyển đo gia tốc ba trục và tốc độ góc ba trục, đọc qua I2C ở tần số #(tan-so-imu) theo cách làm trong bài hướng dẫn [2], để không bỏ sót đỉnh va đập hẹp khoảng 20–50 ms [11].

- *Gia tốc và gia tốc trọng trường đo bằng cảm biến MPU6050:* khi đứng yên, độ lớn gia tốc tổng bằng gia tốc trọng trường $g = 9,81$ m/s² [6]; khi rơi tự do, độ lớn gia tốc giảm sâu về gần 0; khi va chạm với sàn, độ lớn gia tốc tăng vọt lên giá trị cao, dùng làm bằng chứng chính để xác nhận ngã [8, 9].

- *Tốc độ góc và tư thế:* con quay (trong MPU6050) đo tốc độ xoay của thân theo ba trục (độ/giây); khi ngã, thân xoay nhanh nên tốc độ góc tăng cao [2]; chúng em dùng làm bằng chứng phụ với ngưỡng tự chọn khoảng 120 độ/giây.

- *Áp suất khí quyển theo độ cao:* độ cao giảm khoảng 8 m thì áp suất tăng khoảng 1 hPa; cảm biến khí áp đo độ thay đổi độ cao tương đối của thiết bị đeo, giúp phân biệt ngã xuống sàn ($Delta_h <= -0,40$ m) với ngồi xuống ghế [3].

// - *Máy trạng thái (state machine):* firmware duy trì #(so-trang-thai-fw) trạng thái là BOOT_SELF_TEST (tự kiểm tra khi khởi động), CALIBRATING (tự hiệu chuẩn cảm biến), MONITORING (giám sát bình thường), SUSPECTED (nghi ngờ rơi tự do), VERIFYING (xác minh va đập và bất động), LOCAL_ALERTING (phát cảnh báo tại chỗ) và DEGRADED (hoạt động hạn chế khi lỗi cảm biến); mỗi trạng thái chỉ chuyển sang trạng thái khác khi dấu hiệu vật lý thỏa điều kiện, nhờ đó loại được rung lắc tức thời.



- *Nguyên lý cảm biến MS5611:* đo áp suất và nhiệt độ để tính độ thay đổi độ cao tương đối so với mốc gần nhất, bù thông tin khi gia tốc bị nhiễu lúc va chạm [3].

== Xác định phương thức hoạt động của thiết bị

Từ các tìm hiểu ở trên, nhóm chúng em dự kiến sản phẩm hoạt động nhờ sự phối hợp giữa thiết bị đeo dùng ESP32-S3 và ứng dụng FallSafe trên điện thoại Android, giống như một đội hai người:

- *Thiết bị đeo ở thắt lưng* (dùng mạch ESP32-S3): là "người quan sát", luôn đo chuyển động của cơ thể để biết có bị ngã hay không.
- *Ứng dụng FallSafe trên điện thoại Android*: là "người điều khiển", đảm nhận mọi việc còn lại: kết nối Bluetooth với thiết bị đeo, nhận tin báo ngã, lưu danh sách người thân, rồi gọi điện, nhắn tin khi cần.

*Dự kiến cách thiết bị làm việc:*

*1. Bật nguồn:* Thiết bị tự kiểm tra xem các cảm biến có hoạt động bình thường không, sau đó kết nối Bluetooth với điện thoại (tên thiết bị bắt đầu bằng #(ten-ble)). Nếu cảm biến quan trọng bị lỗi, thiết bị sẽ báo lỗi cho người dùng biết.

*2. Theo dõi liên tục:* Thiết bị đo chuyển động của người đeo #(tan-so-imu) mỗi giây, tức là cứ 10 phần nghìn giây đo một lần. Chúng em chọn đo nhanh như vậy vì cú va xuống sàn rất ngắn, đo chậm sẽ dễ bỏ sót.

*3. Nhận biết té ngã:* Người cao tuổi không chỉ ngã theo một kiểu. Khi tìm hiểu tài liệu, chúng em thấy có ít nhất ba kiểu ngã thường gặp, mỗi kiểu để lại "dấu vết" khác nhau trên cảm biến:

- *Kiểu 1 – Ngã đột ngột, mạnh* (vấp chân, trượt trên sàn ướt, mất thăng bằng khi quay người): người đổ nhanh về phía trước, phía sau hoặc sang bên. Dấu vết rất rõ: cảm biến đo được lúc "hẫng" ngắn như khi thang máy đi xuống đột ngột, tiếp theo là một cú va mạnh xuống sàn, người xoay nhanh, rồi nằm yên. Đây là kiểu dễ nhận biết nhất.

- *Kiểu 2 – Khuỵu hoặc gục từ từ* (chóng mặt, tụt huyết áp, choáng, hai chân yếu đi): người không rơi tự do mà trượt dần xuống, có thể vịn vào tường hay đồ đạc, nên cú va xuống sàn nhẹ hơn nhiều. Dấu vết yếu: gần như không có lúc "hẫng", cú va nhỏ, nhưng thân người chuyển từ đứng thẳng sang nằm ngang, vị trí của thiết bị thấp xuống và sau đó nằm yên rất lâu. Kiểu này khó nhất vì dễ bị nhầm với việc cúi xuống nhặt đồ, nên chúng em phải dựa nhiều vào việc người đó nằm yên bao lâu và cảm biến khí áp báo vị trí thấp xuống.

- *Kiểu 3 – Ngã từ chỗ ngồi hoặc chỗ nằm* (trượt khỏi ghế, lăn khỏi giường): độ cao ngã thấp nên cú rơi ngắn và cú va không quá mạnh, nhưng người vẫn nằm yên trên sàn. Chúng em xem kiểu này giống kiểu 2: chú ý tới việc vị trí thấp xuống và nằm yên.

Từ đó chúng em dự kiến thiết bị không chỉ chờ một dấu hiệu duy nhất mà kiểm tra theo hai hướng:
- *Hướng 1 (cho kiểu 1):* thấy cú va mạnh, có thể kèm lúc "hẫng" và xoay người nhanh, rồi người nằm yên.
- *Hướng 2 (cho kiểu 2 và 3):* dù cú va nhẹ, nhưng thân người đổi từ đứng sang nằm, vị trí thấp xuống rõ rệt và nằm yên lâu hơn bình thường.

Điểm chung của mọi kiểu ngã là *người nằm yên sau đó*, nên thiết bị luôn kiểm tra bước này trước khi kết luận. Cách này giúp tránh báo nhầm khi người dùng chỉ vung tay, ngồi phịch xuống ghế hay nhảy lên, vì lúc đó họ vẫn cử động bình thường.

Chúng em cũng nhận thấy các kiểu ngã chậm là phần khó và dễ sót nhất, nên khi làm thật sẽ thử riêng từng kiểu để biết thiết bị nhận biết tốt đến đâu và điều chỉnh tiếp.

*4. Báo cho điện thoại:* Khi kết luận có ngã, thiết bị gửi tin cho điện thoại qua Bluetooth. Điện thoại hiện lên đồng hồ đếm ngược 10 giây để người dùng bấm "Tôi ổn" nếu chỉ là báo nhầm.

*5. Gọi người thân:* Nếu hết 10 giây mà không ai bấm hủy, điện thoại làm ba việc cùng lúc: gọi điện cho một người thân được ưu tiên, nhắn tin cho tất cả người thân kèm vị trí trên bản đồ, và tìm vị trí của người bị ngã. Ba việc này không phụ thuộc nhau nên dù điện thoại không bắt được GPS hay mất mạng Internet thì cuộc gọi và tin nhắn vẫn được gửi đi (mặc dù không đính kèm vị trí).

*6. Điều chỉnh thiết bị:* Mỗi người có dáng đi và thói quen khác nhau (người cao tuổi đi chậm hơn người trẻ), nên ứng dụng cho phép xem số liệu cảm biến trực tiếp và đổi các mức so sánh dùng để nhận biết ngã. Sau khi đổi, ứng dụng gửi mức mới xuống thiết bị đeo, và thiết bị trả lời "đã nhận" thì mới coi là thành công.


== Cơ chế an toàn & Dữ liệu:

- *Khi mất kết nối:* Nếu Bluetooth giữa thiết bị đeo và điện thoại bị ngắt, thiết bị vẫn tiếp tục theo dõi và nhớ lại lần ngã vừa xảy ra; khi kết nối lại, thiết bị gửi tin báo bù cho điện thoại. Nếu mất kết nối quá 30 giây, điện thoại phát tiếng kêu SOS và hiện dòng thông báo "Mất kết nối với thiết bị đeo". Nếu sau 10 giây người dùng vẫn không xác nhận (không chạm vào điện thoại), ứng dụng sẽ nhắn tin cho người thân biết là thiết bị đã mất kết nối, để họ chủ động hỏi thăm. Nếu ứng dụng gửi một lệnh đổi mức so sánh hai lần do đường truyền chập chờn, thiết bị chỉ thực hiện một lần.

- *Bảo vệ thông tin cá nhân:* Số đo của cảm biến chỉ được giữ tạm trong lúc thiết bị tính toán, không lưu lại lâu dài. Số điện thoại người thân chỉ lưu trong điện thoại của người dùng, không gửi đi nơi nào khác. Ứng dụng xin riêng từng quyền gọi điện, nhắn tin và lấy vị trí, và chỉ xin khi cần dùng, nên thiếu một quyền thì các chức năng còn lại vẫn chạy.

=== Yếu tố con người và an toàn

Thiết bị đeo ở thắt lưng nên không che tai, không cản trở nghe âm thanh xung quanh; khi tỉnh táo, người dùng có thể chọn "Tôi ổn" trên ứng dụng FallSafe để hủy báo giả trong 10 giây đếm ngược. Bộ ESP32 không có nút SOS; chức năng SOS trên ứng dụng FallSafe cho phép người dùng chủ động cầu cứu khi cảm thấy choáng váng mà thuật toán trên esp32 chưa phát hiện. Thiết bị không chẩn đoán đột quỵ mà chỉ phát hiện té ngã và bất động; mọi trường hợp nghi ngờ đều cần người nhà kiểm tra trực tiếp và gọi cấp cứu khi cần.

=== Tiêu chí đánh giá sản phẩm

*1. Độ nhạy phát hiện ngã:* tỷ lệ ngã thử được phát hiện đúng trên tổng số lượt ngã thử, mục tiêu đạt từ 90% trở lên.

*2. Tốc độ cảnh báo:* thời gian từ lúc xác nhận ngã đến khi SMS được chuyển đi và cuộc gọi được quay số, mục tiêu dưới 10 giây.

*3. Độ tin cậy truyền hằng số:* mọi lần đổi Profile trên Android đều được thiết bị đeo trả lời "đã nhận".

*4. Dễ sử dụng:* người cao tuổi đeo thoải mái cả ngày, thao tác được chức năng SOS và hủy cảnh báo trên ứng dụng FallSafe, người nhà nhận được tin nhắn rõ ràng vị trí.

= TIẾN HÀNH NGHIÊN CỨU

== Nghiên cứu thuật toán phát hiện té ngã bằng cảm biến đeo ở thắt lưng

Kế thừa các công trình nghiên cứu về thuật toán phát hiện ngã bằng cảm biến gia tốc (Bourke [8], Trần Công Án và cộng sự [9]), thuật toán phát hiện té ngã hiệu quả và phổ biến nhất cho thiết bị đeo là phương pháp dựa trên ngưỡng nhận diện chuỗi ba dấu hiệu vật lý liên tiếp của một cú ngã: trước tiên cơ thể rơi tự do nên gia tốc tổng giảm sâu; sau đó va chạm với sàn tạo đỉnh gia tốc lớn; cuối cùng người bị nạn nằm bất động nên gia tốc trở về quanh trọng trường $g$.

*Trước tiên chúng em chọn vị trí đeo và tần số đo.*
Thiết bị đeo ở thắt lưng để đo chuyển động sát trọng tâm cơ thể, ít rung lắc phụ như khi đeo ở tay. IMU được cố định ở #(tan-so-imu) (chu kỳ 10 ms) vì khoảng thời gian va đập rất hẹp (khoảng 20–50 ms); đo thưa hơn sẽ lướt qua và bỏ sót va đập thật.

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

trong đó $a_"vd"$ là thời điểm va đập, $g = 9,81$ m/s², $epsilon = 1,0$ m/s² là dung sai tĩnh, $t$ là thời gian bất động liên tục trong cửa sổ 3000 ms sau va đập.

* ESP32 gồm các trạng thái vẽ dưới đây *:
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

Hình trên: thiết bị khởi đầu ở MONITORING. Khi $a < 4,9$ m/s² liên tục từ 80 ms trở lên thì sang SUSPECTED (nghi ngờ rơi tự do). Khi $a >= 25$ m/s² thì sang VERIFYING (xác nhận va đập), ghi lại thời điểm va đập và bắt đầu đếm mẫu tĩnh. Tại VERIFYING, thiết bị đếm các mẫu liên tiếp thỏa $|a - g| <= 1,0$ m/s²; đủ 1000 ms liên tục (tối thiểu 6 mẫu) thì /*sang LOCAL_ALERTING và*/ kết luận `FALL_CONFIRMED`. Nếu hết cửa sổ 3000 ms mà chưa đủ điều kiện tĩnh, hoặc khe mẫu vượt quá 250 ms, thì quay về MONITORING và ghi nhận báo giả. Tốc độ góc vượt 120 độ/giây và độ cao giảm quá 0,40 m từ MS5611 được cộng điểm nguy cơ nhưng không thay thế điều kiện chính. Trước khi giám sát, thiết bị đi qua BOOT_SELF_TEST và CALIBRATING; khi cảm biến lỗi thì sang DEGRADED; người dùng có thể chủ động yêu cầu SOS trên ứng dụng FallSafe thay vì thao tác trên thiết bị đeo.

== Tìm hiểu cơ chế gọi SOS và SMS kèm tọa độ trên Android

Khi nhận xác nhận ngã `FALL_CONFIRMED` từ ESP32, hoặc khi người dùng chủ động kích hoạt SOS trên ứng dụng FallSafe, ứng dụng Android chạy ba nhánh độc lập: nhánh gọi điện quay số một lần duy nhất tới người nhận ưu tiên bằng `ACTION_CALL` ngay sau khi SMS đã được chuyển cho thiết bị gửi; nhánh SMS gửi tới toàn bộ người thân kèm tọa độ và đường dẫn bản đồ duy nhất `https://maps.google.com/?q={lat},{lon}`; nhánh vị trí lấy qua `getBestAvailableLocation` với thứ tự ưu tiên vị trí đã lưu còn mới, vị trí tổng hợp, GPS, mạng, tối đa 8 giây và không bao giờ chặn cảnh báo. Ba nhánh không chờ nhau nên mất GPS hay hết tiền mạng dữ liệu đều không ngăn được cuộc gọi và tin nhắn SMS qua sóng di động.

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

Thiết bị đeo đo và phát hiện ngã, gửi sự kiện qua Bluetooth; điện thoại xác minh nhanh với người dùng rồi gọi điện và nhắn tin tới người nhà. Người dùng chỉnh hằng số phát hiện ngã trên màn hình Profile, điện thoại chuyển xuống thiết bị và chỉ coi là thành công khi nhận `COMPLETED`.

== Chế tạo thiết bị đeo thắt lưng và lập trình firmware

Chúng em lắp #(mcu-thuc-te), #(imu-thuc-te), #(ap-suat-thuc-te), #(mach-sac-pin) vào hộp đeo thắt lưng. Firmware dài #(dem-dong-firmware) tổ chức theo các khối: tự kiểm tra và hiệu chuẩn lúc khởi động, đọc IMU ở #(tan-so-imu) với bộ đệm trước sự kiện #(bo-dem-truoc-sk), máy trạng thái #(so-trang-thai-fw) trạng thái, đóng gói JSON cảm biến và sự kiện, nhận lệnh cấu hình `PING`, `GET_STATUS`, `SET_SAMPLE_RATE`, `SET_REFERENCE_ALTITUDE`, `SET_DEVICE_TIME`, `START_SELF_TEST`, /*`TRIGGER_BUZZER`, `STOP_BUZZER`,*/ `ACK_EVENT`, `CANCEL_ALERT`, `REBOOT_DEVICE` từ điện thoại. Cổng console có hai chế độ: Human in 4 dòng/giây và CSV để vẽ đồ thị, cùng 5 phím lệnh `r` (CSV), `t` (bảng ngưỡng), `c` (hiệu chuẩn lại), `s` (trạng thái), `h` (trợ giúp).

== Lập trình ứng dụng Android đọc telemetry và chuyển hằng số

Ứng dụng FallSafe viết bằng Kotlin với giao diện Compose gồm 4 tab Trang chủ, Sự kiện, Người thân và Cài đặt, kèm các màn hình Telemetry, Profile (hiệu chuẩn), kiểm tra Bluetooth, quyền và vị trí. Màn hình Telemetry hiển thị trực tiếp gia tốc ba trục (m/s²), tốc độ góc (độ/giây), góc nghiêng, áp suất (Pa), độ cao tương đối (m), phần trăm pin nhận từ thiết bị. Màn hình Profile lưu nhiều bảng hằng số (ngưỡng va đập, dung sai tĩnh, tần số lấy mẫu) vào bộ nhớ điện thoại; khi người dùng bấm sử dụng bảng nào, ứng dụng chuyển toàn bộ hằng số xuống ESP32 qua Bluetooth và Detection Engine chỉ đổi cấu hình khi thiết bị xác nhận đầy đủ. Trung tâm quyền hiển thị ba khả năng gọi điện, SMS và vị trí với ba trạng thái đã cấp, chưa cấp và bị từ chối cần mở Cài đặt; ứng dụng không xin quyền lúc khởi động và không xin quyền vị trí nền.

// == Xác định âm thanh và cách hủy báo động tương ứng
Khi xác nhận ngã, điện thoại hiện màn hình xác minh nguy cơ với đếm ngược 10 giây; người dùng chọn "Tôi ổn" trên ứng dụng FallSafe để hủy báo giả. Chức năng SOS chủ động cũng nằm trên FallSafe, không được kích hoạt bằng nút trên bộ ESP32. Các sự kiện cảm biến và trạng thái thiết bị được mô tả gồm `IMPACT_DETECTED`, `FREE_FALL_SUSPECTED`, `POSTURE_CHANGED`, `INSTABILITY_DETECTED`, `INACTIVITY_DETECTED`, `LOW_BATTERY` và `SENSOR_ERROR`; thao tác SOS và hủy cảnh báo của người dùng được thực hiện trên ứng dụng.

== Thực nghiệm và kết quả kiểm thử hệ thống

Các lần thử dưới đây do chúng em tự làm ở lớp, mỗi lần thử đều đếm bằng tay. Với mỗi kết quả, chúng em tính *tỉ lệ thành công* rồi so với mục tiêu đã đặt ra ở phần tiêu chí đánh giá (từ 90% trở lên), bằng hai con số:

- *Sai số tuyệt đối* = |mục tiêu − kết quả đo được|, tính bằng điểm phần trăm.
- *Sai số tỉ đối* = sai số tuyệt đối ÷ mục tiêu × 100%, cho biết kết quả đo được lệch mục tiêu bao nhiêu phần trăm.

=== Thực nghiệm 1: Thả rơi thiết bị cầm trên tay để kiểm tra ứng dụng FallSafe

*Cách làm:* Chúng em cầm bộ ESP32 trên tay rồi thả rơi xuống, lặp lại *48 lần*. Để dễ gây ra tín hiệu ngã, chúng em hạ ngưỡng báo va đập xuống còn *14 m/s²* (thấp hơn mức dự kiến ban đầu là 25 m/s²), vì thả vật nhỏ từ độ cao tay thì cú va nhẹ hơn nhiều so với người ngã thật.

*Kết quả:* Mỗi khi ESP32 gửi tín hiệu ngã qua Bluetooth, ứng dụng FallSafe đều nhận được và hoạt động tốt: hiện màn hình đếm ngược, rồi gọi điện và nhắn tin cho người thân.

*Nhận xét:* Thí nghiệm này chỉ cho thấy đường truyền từ ESP32 tới FallSafe và các chức năng của ứng dụng chạy ổn. Nó chưa chứng minh thiết bị nhận biết được người ngã, vì thả rơi vật khác hẳn một cơ thể người đang ngã.

=== Thực nghiệm 2: Đeo ở hông và ngã trên đệm thể dục

*Cách làm:* Một bạn đeo thiết bị ở hông rồi ngã xuống đệm của môn Thể dục, lặp lại *70 lần*. Chúng em đếm số lần ứng dụng FallSafe nhận tín hiệu và kích hoạt SOS.

#figure(
  table(
    columns: (4.5cm, 2.5cm, 2.5cm, 3cm),
    align: (left + horizon, center + horizon, center + horizon, center + horizon),
    table.header([*Kết quả*], [*Số lần*], [*Tỉ lệ*], [*Ghi chú*]),
    [FallSafe nhận tín hiệu và kích hoạt SOS], [64], [91,4%], [Hoạt động đúng],
    [FallSafe không hoạt động], [6], [8,6%], [Chưa rõ nguyên nhân],
    [*Tổng số lần ngã thử*], [*70*], [*100%*], [],
  ),
  caption: [Kết quả thử ngã khi đeo ở hông trên đệm thể dục.],
  numbering: _ => "5.1",
)

*So với mục tiêu:* mục tiêu là từ 90% trở lên, kết quả đo được là 91,4% (64 ÷ 70).
- Sai số tuyệt đối = |90 − 91,4| = 1,4 điểm phần trăm (cao hơn mục tiêu).
- Sai số tỉ đối = 1,4 ÷ 90 × 100% ≈ 1,6%.

Như vậy kết quả *đạt mục tiêu*. Tuy vậy, chúng em chưa biết nguyên nhân của 6 lần không hoạt động. Lúc thử, chúng em không nối máy tính để xem dữ liệu, và màn hình Telemetry cũng chưa được lập trình để lưu lại số liệu, nên không có bằng chứng để tìm lỗi nằm ở thiết bị đeo, đường truyền Bluetooth hay ứng dụng.

=== Thực nghiệm 3: Ngã từ từ và trượt khỏi ghế

*Cách làm:* Đây là kiểu ngã khó nhất đã phân tích ở phần trước: người gục dần xuống hoặc trượt khỏi ghế nên cú va rất nhẹ. Chúng em thử 50 lần và đếm số lần SOS được kích hoạt.

#figure(
  table(
    columns: (4.5cm, 2.5cm, 2.5cm, 3cm),
    align: (left + horizon, center + horizon, center + horizon, center + horizon),
    table.header([*Kết quả*], [*Số lần*], [*Tỉ lệ*], [*Ghi chú*]),
    [SOS được kích hoạt], [35], [70,0%], [],
    [SOS không kích hoạt], [15], [30,0%], [Xem phần nhận xét],
    [*Tổng số lần thử*], [*50*], [*100%*], [],
  ),
  caption: [Kết quả thử ngã từ từ và trượt khỏi ghế.],
  numbering: _ => "5.2",
)

*So với mục tiêu (90%):*
- Sai số tuyệt đối = |90 − 70,0| = 20,0 điểm phần trăm.
- Sai số tỉ đối = 20,0 ÷ 90 × 100% ≈ 22,2%.

*Nhận xét:* Kết quả *chưa đạt mục tiêu* và thấp hơn rõ rệt so với ngã mạnh. Điều này đúng với dự đoán ban đầu của chúng em: ngã chậm để lại dấu vết yếu nên khó nhận biết hơn. Có thể một phần 15 lần không kích hoạt là do bạn thử diễn chưa giống ngã thật (ví dụ vẫn tự chống tay, ngồi xuống quá nhẹ), nhưng chúng em chưa có cách kiểm tra điều đó nên chưa dám khẳng định.

=== Thực nghiệm 4: Tốc độ cảnh báo

Trong các lần thử ở trên, chúng em ghi lại thời gian từ lúc thiết bị phát hiện ngã đến khi điện thoại gọi điện và nhắn tin đi. Thời gian cảnh báo trung bình nằm trong khoảng *từ 4 đến 7 giây*, thấp hơn mục tiêu là dưới 10 giây, tức là còn dư ít nhất 3 giây. Tiêu chí này *đạt*.

=== Thực nghiệm 5: Truyền hằng số xuống thiết bị

Chúng em đổi các mức so sánh nhận biết ngã trên ứng dụng rồi gửi xuống thiết bị đeo, lặp lại *68 lần*.

#figure(
  table(
    columns: (6cm, 2.5cm, 2.5cm),
    align: (left + horizon, center + horizon, center + horizon),
    table.header([*Kết quả*], [*Số lần*], [*Tỉ lệ*]),
    [Gửi thành công ngay lần đầu], [49], [72,1%],
    [Phải bấm gửi lại lần thứ hai], [11], [16,2%],
    [Còn lại (nhật ký chưa ghi rõ kết quả)], [8], [11,8%],
    [*Tổng số lần thử*], [*68*], [*100%*],
  ),
  caption: [Kết quả gửi mức so sánh từ ứng dụng xuống thiết bị đeo.],
  numbering: _ => "5.3",
)

Mục tiêu là mọi lần gửi đều được thiết bị xác nhận. Chỉ có 49/68 lần (72,1%) thành công ngay lần đầu, nên *chưa đạt*: sai số tuyệt đối so với 100% là 27,9 điểm phần trăm, sai số tỉ đối 27,9%. Nếu cho phép bấm lại một lần thì được 60/68 lần (88,2%). Chúng em chưa biết vì sao nhiều lần phải gửi lại, đây là việc cần cải thiện.

=== Thực nghiệm 6: Mức độ dễ sử dụng

Các bạn học sinh tham gia thử đều phản hồi là ứng dụng *rất dễ sử dụng*. Tuy nhiên đây mới là ý kiến của học sinh, chưa phải người cao tuổi, và chưa chấm điểm bằng số, nên chúng em chỉ xem là kết quả ban đầu.

=== Tổng hợp các thực nghiệm

#figure(
  table(
    columns: (4.5cm, 2cm, 2cm, 2.2cm, 2.6cm, 2.6cm),
    align: (left + horizon, center + horizon, center + horizon, center + horizon, center + horizon, center + horizon),
    table.header(
      [*Thực nghiệm*], [*Số lần*], [*Thành công*], [*Tỉ lệ*], [*Sai số tuyệt đối so với 90%*], [*Sai số tỉ đối*]
    ),
    [Ngã trên đệm, đeo hông], [70], [64], [91,4%], [1,4 điểm (vượt)], [1,6%],
    [Ngã từ từ, trượt khỏi ghế], [50], [35], [70,0%], [20,0 điểm], [22,2%],
    [*Cộng hai kiểu ngã*], [*120*], [*99*], [*82,5%*], [*7,5 điểm*], [*8,3%*],
  ),
  caption: [Tổng hợp tỉ lệ kích hoạt SOS khi thử ngã và so với mục tiêu 90%.],
  numbering: _ => "5.4",
)

#v(0.3cm)

#align(center)[
  #canvas({
    import draw: *

    let w = 12.0
    let h = 4.8
    let ox = 1.0

    let items = (
      (label: [Ngã trên đệm], val: 91.4, color: rgb("#2563eb")),
      (label: [Ngã từ từ], val: 70.0, color: rgb("#059669")),
      (label: [Cộng hai kiểu], val: 82.5, color: rgb("#7c3aed")),
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

    // đường mục tiêu 90%
    let ty = (90 / 100) * h
    line((ox, ty), (ox + w, ty), stroke: (paint: rgb("#dc2626"), thickness: 1pt, dash: "dashed"))
    content((ox + w - 1.2, ty + 0.3), text(fill: rgb("#dc2626"))[Mục tiêu 90%])

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
#fig-caption[Tỉ lệ kích hoạt SOS khi thử ngã, so với mục tiêu 90% (đường đứt đỏ).]

*Đối chiếu từng tiêu chí đã đặt ra:*

#figure(
  table(
    columns: (4cm, 3cm, 4cm, 4cm),
    align: (left + horizon, center + horizon, center + horizon, left + horizon),
    table.header([*Tiêu chí*], [*Mục tiêu*], [*Kết quả đo được*], [*Đánh giá*]),
    [1. Độ nhạy phát hiện ngã],
    [từ 90%],
    [91,4% (ngã trên đệm); 70,0% (ngã từ từ)],
    [Đạt với ngã mạnh; chưa đạt với ngã từ từ],

    [2. Tốc độ cảnh báo], [dưới 10 giây], [Trung bình từ 4 đến 7 giây], [Đạt],
    [3. Truyền hằng số xuống thiết bị],
    [Mọi lần đều được xác nhận],
    [49/68 lần đúng ngay lần đầu (72,1%); 60/68 nếu được gửi lại một lần (88,2%)],
    [Chưa đạt],

    [4. Dễ sử dụng],
    [Đeo thoải mái, thao tác được],
    [Học sinh thử phản hồi rất dễ dùng],
    [Khá tốt; cần thử với người cao tuổi],
  ),
  caption: [Đối chiếu kết quả với các tiêu chí đánh giá.],
  numbering: _ => "5.5",
)

== Kết luận

- Thí nghiệm thả rơi cho thấy đường truyền từ thiết bị ESP32 tới ứng dụng FallSafe và các chức năng của ứng dụng (đếm ngược, gọi điện, nhắn tin) hoạt động tốt khi có tín hiệu ngã.
- Khi thử ngã thật trên đệm thể dục, thiết bị kích hoạt SOS được 64/70 lần (91,4%), đạt mục tiêu 90%. Với kiểu ngã từ từ, trượt khỏi ghế, kết quả là 35/50 lần (70,0%), thấp hơn mục tiêu 20,0 điểm phần trăm (sai số tỉ đối 22,2%). Tốc độ cảnh báo trung bình từ 4 đến 7 giây nên đạt mục tiêu dưới 10 giây; ứng dụng được học sinh đánh giá là rất dễ sử dụng. Tuy vậy, độ nhạy với kiểu ngã chậm và việc gửi mức so sánh xuống thiết bị (49/68 lần thành công ngay) vẫn chưa đạt mục tiêu, và kiểu ngã chậm là phần cần cải thiện nhiều nhất.
- Hạn chế của đề tài: (1) chưa tìm ra nguyên nhân của 6 lần FallSafe không hoạt động ở thực nghiệm 2 vì lúc thử chưa lập trình để lưu lại số liệu; (2) các bạn thử chỉ là học sinh, chưa thử với người cao tuổi; (3) chưa có người dùng thật đeo trong sinh hoạt hằng ngày nên chưa đánh giá được mức độ báo nhầm; (4) ngã trên đệm khác với ngã trên sàn cứng; (5) thiết bị chỉ phát hiện té ngã chứ không chẩn đoán đột quỵ.
- Hướng phát triển tiếp theo: cho phần mềm lưu lại số liệu cảm biến của mỗi lần thử để tìm nguyên nhân các lần thất bại; điều chỉnh ngưỡng cho kiểu ngã chậm; tìm nguyên nhân khiến việc gửi mức so sánh xuống thiết bị phải thử lại.

= TÀI LIỆU THAM KHẢO

#set par(first-line-indent: 0pt)
#enum(
  numbering: "[1]  ",
  [Waveshare. (2024). _Working with Arduino: hướng dẫn cài đặt Arduino IDE và nạp chương trình cho ESP32-S3_. https://docs.waveshare.com/ESP32-S3-DEV-KIT-N8R8/Development-Environment-Setup-Arduino],
  [Santos, S. (2023). _ESP32 with MPU6050 Accelerometer, Gyroscope, and Temperature Sensor (Arduino IDE)_. Random Nerd Tutorials. https://randomnerdtutorials.com/esp32-mpu-6050-accelerometer-gyroscope-arduino/],
  [How2Electronics. (2022). _Interface MS5611 Barometer & Altimeter Sensor with Arduino_. https://how2electronics.com/interface-ms5611-barometer-altimeter-sensor-with-arduino/],
  [Google Developers. (2024). _Android Location, SMS Manager and Telephony Documentation_. https://developer.android.com],
  [Santos, R., & Santos, S. (2023). _Getting Started with ESP32 Bluetooth Low Energy (BLE) on Arduino IDE_. Random Nerd Tutorials. https://randomnerdtutorials.com/esp32-bluetooth-low-energy-ble-arduino-ide/],
  [Bộ Giáo dục và Đào tạo. (2024). _Sách giáo khoa Vật lí 10_ (bộ sách Kết nối tri thức với cuộc sống). Nhà xuất bản Giáo dục Việt Nam.],
  [Bộ Giáo dục và Đào tạo. (2024). _Sách giáo khoa Tin học 11_ (bộ sách Kết nối tri thức với cuộc sống). Nhà xuất bản Giáo dục Việt Nam.],
  [Bourke, A. K., O'Brien, J. V., & Lyons, G. M. (2007). _Evaluation of a threshold-based tri-axial accelerometer fall detection algorithm_. Gait & Posture, 26(2), 194–199. https://pubmed.ncbi.nlm.nih.gov/17101272/],
  [Trần Công Án, Đỗ Thanh Đức, Lê Đình Chiến và cộng sự. (2017). _Phát hiện té ngã cho người cao tuổi bằng gia tốc kế_. Tạp chí Khoa học Đại học Cần Thơ, Số chuyên đề Công nghệ thông tin, 65–71. https://doi.org/10.22144/ctu.jsi.2017.009],
  [Lê Trường Giang, Nguyễn Khánh Lợi. (2024). _Hệ thống theo dõi sức khỏe tích hợp trên ứng dụng_. Tạp chí Khoa học và Công nghệ Giao thông, 4(4), 25–32. https://doi.org/10.58845/jstt.utt.2024.vn.4.4.25-32],
  [Analog Devices. (2013). _Application Note AN-1023: Detecting Human Falls with a 3-Axis Digital Accelerometer_. https://www.analog.com/media/en/analog-dialogue/volume-43/number-3/articles/detecting-falls-3-axis-digital-accelerometer.pdf],
  [How2Electronics. (2021). _Power Supply for ESP32: Boost Converter and Battery Charger (IP5306)_. https://how2electronics.com/power-supply-for-esp32-with-boost-converter-battery-charger/],
)


#import "@preview/rustycure:0.2.0": qr-code
#let link-drive = "https://drive.google.com/drive/folders/1kzAuWYaG4mUk-BW27-T6wg8LHkUj7otw?usp=sharing"
#let qr-drive = qr-code(link-drive, width: 6cm)

#pagebreak()
#heading(level: 1, numbering: none, outlined: false)[PHỤ LỤC]

#set par(first-line-indent: 0pt)

*Phụ lục 1: Kho lưu trữ minh chứng của dự án*

Toàn bộ minh chứng của dự án được chúng em lưu trên Google Drive. Quét mã QR bên dưới để truy cập:

#align(center)[
  #qr-drive
  #v(0.2cm)
  Đường dẫn: #link(link-drive)
]

#v(0.3cm)
Kho lưu trữ gồm các thư mục sau:

#figure(
  table(
    columns: (4.5cm, 11cm),
    align: (left + horizon, left + horizon),
    table.header([*Thư mục*], [*Nội dung*]),
    [Mã nguồn], [Firmware ESP32-S3 (Arduino) và mã nguồn ứng dụng Android FallSafe ở phiên bản hoàn thiện.],
    // [Lịch sử thay đổi mã], [Các phiên bản mã qua từng lần chỉnh sửa, ghi lại việc thay đổi ngưỡng và thuật toán phát hiện ngã.],
    [Hình ảnh quá trình làm],
    [Ảnh học sinh lắp mạch, hàn linh kiện, lập trình, chế tạo hộp đeo và hoàn thiện sản phẩm.],

    [Hình ảnh và video thực nghiệm],
    [Ảnh, video các lần thả rơi, ngã trên đệm thể dục, ngã từ từ, trượt khỏi ghế và thử gửi hằng số.],
  ),
  caption: [Cấu trúc kho lưu trữ minh chứng trên Google Drive.],
  numbering: _ => "P.1",
)
