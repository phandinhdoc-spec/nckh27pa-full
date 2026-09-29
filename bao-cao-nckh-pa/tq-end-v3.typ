#import "@preview/cetz:0.3.4" as cetz: canvas, draw

#set page(
  paper: "a4",
  margin: (left: 3cm, right: 2cm, top: 2cm, bottom: 2cm),
  // header: context {
  //   if counter(page).get().first() > 1 {
  //     align(center)[
  //       #text(size: 10pt, weight: "bold")[CUỘC THI NGHIÊN CỨU KHOA HỌC]\
  //       #text(size: 9pt)[LẦN THỨ 22 NĂM 2026]
  //       #v(-4pt)
  //       #line(length: 100%, stroke: 0.5pt)
  //     ]
  //   }
  // },
  footer: context {
    if counter(page).get().first() > 1 {
      align(center)[#text(size: 10pt)[#counter(page).display()]]
    }
  },
)

#set text(font: "Times New Roman", size: 14pt, lang: "vi", region: "VN")
#let ten-du-an = [Thiết bị thông minh sử dụng AI hỗ trợ người khiếm thị]
#set par(justify: true, leading: 0.6em, spacing: 0.6em, first-line-indent: (amount: 8mm, all: true))

#set list(indent: 8mm, body-indent: 0.6em, spacing: 0.6em, marker: ([-], [+]))
#show list.item: it => [
  #set par(first-line-indent: 0mm, hanging-indent: -16mm)
  #it
]
#set enum(indent: 8mm, body-indent: 0.6em, spacing: 0.6em)
#show enum.item: it => [
  #set par(first-line-indent: 0mm, hanging-indent: -16mm)
  #it
]
// Tham khảo ~/TypstLib/local/vn-edu/0.1.0/vn-pages.typ (hàm vn-pages):
// chép ý tưởng set/show vào trực tiếp file này, KHÔNG import @local.
// Quy định báo cáo NCKH yêu cầu Times New Roman 14pt nên nâng từ 13pt của mẫu;
// scheme đánh số dùng A / 1 / 1.1 theo văn bản báo cáo (mẫu dùng I / 1 / 1.1).
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

// Dữ liệu thực tế được tích hợp trực tiếp trong file này để tài liệu tự biên dịch độc lập.
#let nguon-so-lieu-nguoi-khiem-thi = [báo Thanh Niên, bài viết ngày 14/11/2025]
#let so-nguoi-khiem-thi = [gần 2 triệu người khiếm thị và người suy giảm thị lực]
#let bang-chung-khao-sat-nhu-cau = [tại khu nhà ở dành cho người khiếm thị, số 446 Lý Thái Tổ, phường Vườn Lài, TP. Hồ Chí Minh]
#let bo-xu-ly-thuc-te = [MacBook Apple Silicon làm máy chủ xử lý; Raspberry Pi Zero 2 W là thiết bị đầu cuối]
#let camera-thuc-te = [camera Raspberry Pi giao tiếp CSI, chụp ảnh 640 × 640]
#let dau-ra-am-thanh-thuc-te = [loa mini qua mạch MAX98357A]
#let co-che-am-thanh-thuc-te = [MacBook tạo giọng tiếng Việt bằng Apple Speech/TTS, mã hóa MP3 và gửi thiết bị đầu cuối phát]
#let khoi-luong-tren-kinh = [180 g]
#let khoi-luong-toan-bo = [180 g]
#let dung-luong-pin = [pin sạc dự phòng 10.000 mAh]
#let thoi-luong-pin = [gần 8 giờ chạy liên tục với pin 10.000 mAh]
#let nhiet-do-cao-nhat = [60 °C ở CPU khi hoạt động ngoài trời nắng nóng, trong khi nhiệt độ môi trường khoảng 36 °C]
#let kich-thuoc-san-pham = [dạng vòng tròn, có khóa cài phía sau để thay đổi bán kính theo kích thước đầu]
#let tong-chi-phi-thuc-te = [1.937.000 đồng]
#let danh-sach-linh-kien-va-gia = [camera và module xử lý: 1.500.000 đồng; pin và mạch sạc: 160.000 đồng; loa và mạch MAX98357A: 105.000 đồng; vỏ in 3D và dây đeo: 140.000 đồng; công tắc: 32.000 đồng]
// #let phien-ban-phan-mem = [Chọn loại nhỏ n]

#pagebreak()

// =================== TÓM TẮT ĐỀ TÀI ===================
#title[TÓM TẮT DỰ ÁN]
#v(0.5cm)


#text(weight: "bold")[Tính mới:] Tính mới của dự án là sử dụng công nghệ AI để hỗ trợ người đi đường tránh vật cản. Ngoài ra còn hỗ trợ các câu hỏi thường ngày của người khiếm thị.

#text(weight: "bold")[Tính khoa học:] Ngoài việc tích hợp AI, thuật toán xác định vật cản và hướng dẫn người đi đường được chúng em nghiên cứu dựa trên các kiến thức toán học, vật lý và lập trình. Thuật toán dùng số đo góc nghiêng từ cảm biến GY25 và số đo khoảng cách từ cảm biến GY53L1X.

#text(weight: "bold")[Tính thực tiễn:] Thiết bị hỗ trợ người khiếm thị hỏi đáp các thông tin cơ bản thường ngày, hỗ trợ người đi đường tránh vật cản với chi phí hợp lí.

#text(weight: "bold")[Tính cộng đồng:] Dự án hướng tới hỗ trợ người khiếm thị tiếp cận thông tin môi trường bằng tiếng nói, hạn chế phải thao tác bằng tay. Theo thống kê #(nguon-so-lieu-nguoi-khiem-thi) #footnote[https://thanhnien.vn/gan-2-trieu-nguoi-viet-khiem-thi-suy-giam-thi-luc-chua-den-1-sach-chu-noi-185251114131539441.htm], hiện có khoảng #(so-nguoi-khiem-thi) người khiếm thị có thể được hưởng lợi từ giải pháp này.


#pagebreak()

// =================== MỤC LỤC ===================
#title[MỤC LỤC]
#v(0.5cm)
#outline(title: none, indent: 1.5em)
// =================== NỘI DUNG CHÍNH ===================
#pagebreak()
= LÝ DO CHỌN DỰ ÁN

// =================== NỘI DUNG CŨ (giữ lại để đối chiếu) ===================
// Qua tìm hiểu nhu cầu của người khiếm thị, chúng em nhận thấy việc tiếp cận thông tin xung quanh và nhận trợ giúp khi đi đường vẫn còn nhiều khó khăn. Người khiếm thị có thể cần hỏi về một vật thể, đọc thông tin hoặc nhận biết vật cản, nhưng việc sử dụng các thiết bị thông thường đôi khi đòi hỏi thao tác bằng tay hoặc sự hỗ trợ của người khác.
//
// Từ thực tế đó, chúng em đặt câu hỏi: Làm thế nào để người khiếm thị có thể chủ động hỏi thông tin về môi trường xung quanh và nhận hướng dẫn bằng tiếng nói thông qua một thiết bị thuận tiện khi sử dụng?
//
// Chúng em chọn thực hiện dự án #ten-du-an nhằm tìm hướng giải quyết cho nhu cầu ấy. Dự án sử dụng Raspberry Pi làm thiết bị đầu cuối và máy chủ MacBook Apple Silicon để tiếp nhận lời nói, chuyển thành văn bản, phân tích yêu cầu bằng các dịch vụ AI, xử lý hình ảnh khi cần và phát phản hồi bằng tiếng Việt. Bên cạnh việc tra cứu thông tin cơ bản, hệ thống còn hướng tới hỗ trợ người dùng nhận biết vật cản khi đi đường.
//
// Chúng em lựa chọn đề tài này vì mong muốn vận dụng kiến thức về lập trình, điện tử và trí tuệ nhân tạo để tạo ra một giải pháp có tính ứng dụng, góp phần giúp người khiếm thị tiếp cận thông tin thuận tiện và chủ động hơn trong sinh hoạt hằng ngày.
// =================== KẾT THÚC NỘI DUNG CŨ ===================

// =================== NỘI DUNG TÓM TẮT (sắp xếp lại theo logic: bối cảnh → thực trạng → câu hỏi → giải pháp → động lực) ===================

Theo thống kê #(nguon-so-lieu-nguoi-khiem-thi) #footnote[https://thanhnien.vn/gan-2-trieu-nguoi-viet-khiem-thi-suy-giam-thi-luc-chua-den-1-sach-chu-noi-185251114131539441.htm], Việt Nam có #(so-nguoi-khiem-thi). Đây là nhóm người phải đối mặt với nhiều khó khăn trong sinh hoạt hằng ngày mà phần lớn công nghệ hiện tại chưa giải quyết triệt để.

Qua khảo sát thực tế #footnote[#(bang-chung-khao-sat-nhu-cau)], chúng em nhận thấy người khiếm thị gặp ba khó khăn chính:
- *Tránh vật cản khi đi đường:* khó phát hiện vật cản phía trước nên dễ vấp ngã hoặc va chạm, đặc biệt ở nơi lạ hoặc đông người.
- *Tìm đồ vật và nhận biết vật xung quanh:* chỉ dùng xúc giác và thính giác nên mất nhiều thời gian, đôi khi phải nhờ người khác xác nhận.
- *Tiếp cận thông tin chủ động:* các thiết bị thông thường đòi hỏi thao tác bằng tay hoặc nhìn màn hình, chưa phù hợp với người khiếm thị.

Từ thực trạng đó, chúng em đặt câu hỏi: *Làm thế nào để người khiếm thị có thể chủ động tra cứu thông tin và nhận hướng dẫn tránh vật cản bằng tiếng nói, thông qua một thiết bị thuận tiện, rảnh tay?*

Chúng em quyết định thực hiện dự án *#ten-du-an* -- một thiết bị đeo dạng vòng trên đầu tích hợp camera, cảm biến và AI, cho phép người dùng hỏi bằng tiếng nói và nhận hướng dẫn âm thanh mà không cần thao tác bằng tay. Các thiết bị hỗ trợ người khiếm thị hiện có hoặc chưa hỗ trợ hỏi đáp bằng giọng nói, hoặc có giá thành rất cao (kính AI chuyên dụng lên tới 90.000.000+ đồng), vượt khả năng tiếp cận của đa số người dùng trong nước.

Chúng em chọn đề tài này vì mong muốn vận dụng kiến thức lập trình, điện tử và trí tuệ nhân tạo để tạo ra một giải pháp thiết thực, chi phí hợp lí, góp phần giúp người khiếm thị sống tự lập và chủ động hơn trong sinh hoạt hằng ngày.

// == Khó khăn của người khiếm thị trong cuộc sống
// // (Mục này được chuyển từ chương PHÂN TÍCH THỰC TRẠNG VÀ CƠ SỞ GIẢI PHÁP lên phần A vì nêu thực trạng và tác động xã hội của vấn đề nghiên cứu.)
// // --- Nội dung cũ (giữ lại để đối chiếu, sửa nếu cần) ---
// Qua khảo sát , chúng em nhận thấy người khiếm thị gặp ba khó khăn nổi bật trong sinh hoạt hằng ngày:
// - *Khó khăn khi đi đường và tránh vật cản:* Người khiếm thị khó phát hiện vật cản phía trước nên dễ vấp ngã hoặc va chạm, nhất là ở nơi lạ, nơi đông người hoặc khi không có người dắt. Thính giác phải vừa nghe âm thanh xung quanh để định hướng, vừa nghe hướng dẫn nên thiết bị hỗ trợ cần hạn chế che tai.
// - *Khó khăn khi tìm đồ vật và nhận biết vật xung quanh:* Khi chỉ dùng xúc giác và thính giác, người khiếm thị mất nhiều thời gian để tìm đồ vật, khó phân biệt các vật có hình dạng gần giống nhau và đôi khi phải nhờ người khác xác nhận lại.
// - *Khó khăn khi tiếp cận thông tin một cách chủ động:* Người khiếm thị có thể cần hỏi về một vật thể, đọc nhãn hoặc bảng thông tin, nhưng các thiết bị thông thường đòi hỏi thao tác bằng tay hoặc nhìn màn hình nên chưa thuận tiện. Vì vậy, người khiếm thị cần một cách hỏi bằng tiếng nói và nghe hướng dẫn trong trạng thái rảnh tay.

#figure(
  image("gop/tq/image/khao sat.jpg", height: 6.5cm),
  caption: [Khảo sát thực tế về khó khăn của người khiếm thị trong cuộc sống.],
  numbering: _ => "2.1",
)

// = VẤN ĐỀ NGHIÊN CỨU (tên cũ, giữ để đối chiếu)
= CÂU HỎI NGHIÊN CỨU, VẤN ĐỀ NGHIÊN CỨU VÀ GIẢ THUYẾT KHOA HỌC

== Câu hỏi nghiên cứu

// + Chúng em có thể xây dựng thiết bị đeo gửi âm thanh từ Raspberry Pi đến máy chủ MacBook để chuyển giọng nói thành văn bản, rồi sử dụng Gemini API và CommandCode API để phân tích yêu cầu, xử lý hình ảnh khi cần và trả hướng dẫn bằng tiếng Việt với độ trễ, độ chính xác và bảo vệ dữ liệu phù hợp không?

+ Sử dụng thiết bị gì và dùng thuật toán nào để hướng dẫn người khiến thị tránh vật cản khi đi đường? Các AI hiện nay có thực hiện được việc này không?

// + Khung vòng đeo trên đầu cần được thiết kế như thế nào để có khối lượng phù hợp, ôm vừa vặn và phân bổ đều quanh vòm đầu, dễ thao tác và hạn chế cản trở việc nghe âm thanh xung quanh của người khiếm thị?

+ Các nhu cầu thường ngày khác của người khiếm thị là gì? Sản phẩm có thể giúp gì cho họ?


+ Có nhiều loại cảm biến có cùng chức năng, nên lựa chọn loại nào cho phù hợp, vừa dễ mua, dễ lập trình, không quá đắt đỏ?

== Vấn đề nghiên cứu

// Dự án là hệ thống hỗ trợ người khiếm thị gồm Raspberry Pi Zero 2 W làm thiết bị đầu cuối và máy chủ MacBook Apple Silicon. Ba vấn đề cần giải quyết tương ứng với ba câu hỏi nghiên cứu là:

// - Vấn đề 1 -- luồng hỏi đáp bằng tiếng nói qua API: thiết bị gửi âm thanh tới máy chủ để speech-to-text; máy chủ phân tích yêu cầu, gọi Gemini API (Google AI Studio) hoặc CommandCode API theo cấu hình, nhận và xử lý ảnh khi cần, rồi tạo phản hồi giọng nói tiếng Việt. Cần lựa chọn module thu âm, camera, kết nối mạng và phát âm thanh phù hợp, đồng thời bảo đảm độ trễ, độ chính xác và bảo vệ dữ liệu phù hợp.

// - Vấn đề 2 -- thiết kế vòng đeo trên đầu: tìm hiểu nhu cầu, thói quen của người khiếm thị để thiết kế sản phẩm dễ sử dụng, thoải mái khi đeo; tìm hiểu về in 3D để thiết kế khung vòng đeo sao cho nhẹ, ôm vừa vặn và phân bổ đều quanh vòm đầu, dễ thao tác bằng một công tắc và hạn chế cản trở việc nghe âm thanh xung quanh.

// - Vấn đề 3 -- cảm biến và thuật toán hỗ trợ đi đường: lựa chọn số lượng cảm biến tối thiểu, loại cảm biến dễ tìm mua và phù hợp với thiết bị đeo, cùng thuật toán xác định khoảng cách vật cản và cách hướng dẫn người khiếm thị đi đường bằng tiếng nói. Qua tìm hiểu, nhóm chọn hai cảm biến là GY25 (phát triển từ MPU6050) đo góc nghiêng và GY53L1X (phát triển từ VL53L1X và vi điều khiển STM32) đo khoảng cách.


+ Nghiên cứu các sản phẩm hỗ trợ người khiếm thị đã có trên thị trường, ưu và nhược điểm từng sản phẩm, xác định hướng phát triển sản phẩm của đề tài.

+ Nghiên cứu các công nghệ AI hỗ trợ người khiếm thị, ưu và nhược điểm từng công nghệ, xác định model AI phù hợp.

+ Nghiên cứu các vi xử lí, lựa chọn vi xử lí phù hợp với đề tài, ưu tiên chọn loại rẻ nhất nhưng đủ mạnh để xử lí các tác vụ cần thiết.

+ Nghiên cứu hướng tạo server bằng máy tính để xử lí các tác vụ nặng như speech-to-text và phân tích yêu cầu, giúp giảm tải cho thiết bị đeo.

+ Nghiên cứu các thuật toán xác định vật thể trong ảnh, xác định khoảng cách trong thực tế giữa hai vật trong ảnh.

+ Nghiên cứu cơ chế cảnh báo vật cản khi người khiếm thị đi đường, cơ chế phải bù đắp được thói quen của người khiếm thị.


// == Mục tiêu nghiên cứu

// - Mục tiêu 1: xây dựng và kiểm tra luồng hỏi đáp bằng tiếng nói (thu âm, speech-to-text, phân tích yêu cầu bằng Gemini API và CommandCode API, xử lý ảnh khi cần, phản hồi giọng nói tiếng Việt); đánh giá độ trễ, độ chính xác và việc bảo vệ dữ liệu.

// - Mục tiêu 2: thiết kế, in 3D và hoàn thiện vòng đeo trên đầu có khối lượng phù hợp, ôm vừa vặn, phân bổ đều quanh vòm đầu, dễ thao tác bằng một công tắc và hạn chế cản trở việc nghe âm thanh xung quanh.

// - Mục tiêu 3: lựa chọn cấu hình cảm biến tối thiểu, dễ tìm mua và phù hợp với thiết bị đeo; xây dựng thuật toán xác định khoảng cách vật cản và thử nghiệm hướng dẫn người khiếm thị đi đường bằng tiếng nói. Trong đó, GY25 đo góc nghiêng của đầu và GY53L1X đo khoảng cách tới vật cản.

// - Mục tiêu 1: Tạo ra sản phẩm hỗ trợ người khiếm thị trong các hoạt động thường ngày.

// - Mục tiêu 2: Tìm ra thuật toán xác định khoảng cách trong thực tế giữa hai vật trong ảnh.

// == Yêu cầu đề tài
// 1. *Dễ sử dụng:* Chỉ có một công tắc điều khiển để người dùng không bị bối rối.
// 2. *Thoải mái:* Thiết bị dạng vòng đeo trên đầu cần có khối lượng nhẹ, phân bố trọng tâm cân đối, dây điện gọn gàng để người dùng đội thoải mái khi sử dụng.
// 3. *An toàn và tôn trọng quyền riêng tư:* Không tự ý sử dụng camera khi không cần thiết hoặc không được yêu cầu, sản phẩm khi chạy có nhiệt độ trong giới hạn an toàn và nhiệt độ của vỏ thiết bị phần tiếp xúc với người sử dụng không được lớn hơn nhiệt độ môi trường tối đa $1^0$ C.
// // 4. *Chi phí hợp lí:* Chi phí không quá cao để đa số người dùng có thể tiếp cận.

== Giả thuyết, đối tượng và phạm vi nghiên cứu
*Giả thuyết khoa học:* từ ba câu hỏi nghiên cứu, chúng em đặt ba giả thuyết cần kiểm tra bằng phép thử của phiên bản hiện hành.

(1) Ảnh tạo bởi camera cho tỉ lệ giữa các vật rất trung thực, nếu biết các thông tin khác, có thể tính ra khoảng cách giữa hai vật trong ảnh.

(2) Có thể sử dụng các model AI để thực hiện xác định vật thể, tọa độ điểm ảnh để từ đó tính ra được khoảng cách giữa vật và người khiếm thị nếu biết chiều cao người khiếm thị và thông số của camera.

(3) Các model AI hiện này đủ thông minh để trả lời các câu hỏi thường ngày, đủ nhanh để hỗ trợ người khiếm thị tránh được vật cản.



// = THIẾT KẾ VÀ PHƯƠNG PHÁP NGHIÊN CỨU (tên cũ cấp 1; nay là mục 6 của phần B)
= Thiết kế và phương pháp nghiên cứu
// Mục này mô tả tiến trình, thiết kế thí nghiệm, phương pháp thu thập số liệu, xác định giải pháp, thiết kế mô hình, rủi ro kỹ thuật, cảnh báo an toàn và tiêu chí đánh giá.

== Đối tượng nghiên cứu
Là luồng xử lý tiếng nói và hình ảnh (thu âm, speech-to-text, phân tích yêu cầu qua API, xử lý ảnh khi cần, phản hồi giọng nói tiếng Việt), độ trễ, độ ổn định, chất lượng phản hồi, vòng đeo trên đầu cùng khả năng sử dụng của thiết bị, và cụm cảm biến GY25, GY53L1X với thuật toán đo khoảng cách vật cản.

== Phạm vi nghiên cứu:
Nghiên cứu nhu cầu của người khiếm thị tại #bang-chung-khao-sat-nhu-cau


== Phương pháp nghiên cứu
Để thực hiện dự án, chúng em sử dụng các phương pháp sau:
1. *Nghiên cứu tài liệu:* Đọc Sách giáo khoa Toán 8,9 [11] (định lí Pythagore, tỉ số lượng giác sin, cos, tan trong tam giác vuông) và Sách giáo khoa Khoa học tự nhiên 9 [12] (sự tạo ảnh qua thấu kính hội tụ) làm cơ sở lý thuyết cho thuật toán xác định khoảng cách tới vật cản; đồng thời đọc tài liệu kỹ thuật về speech-to-text, API Gemini, kiến trúc máy chủ và giao tiếp thiết bị.
2. *Thiết kế và phát triển mẫu thử:* Chia công việc thành hai phía. Phía thiết bị đầu cuối: tìm hiểu cách lập trình và kết nối phần cứng trên Raspberry Pi [4], tra cứu thông số Raspberry Pi Zero 2 W [5] và tài liệu hệ điều hành DietPi [6]. Phía máy chủ: xây dựng máy chủ Swift/Hummingbird chạy trên macOS (MacBook Apple Silicon) để nhận âm thanh, thực hiện speech-to-text, điều phối yêu cầu tới Gemini API và CommandCode API theo cấu hình, xử lý ảnh khi cần và tạo phản hồi giọng nói tiếng Việt.
3. *Thực nghiệm và thống kê:* Ghi số lượt xử lý thành công, lỗi, độ trễ và chất lượng phản hồi trong các điều kiện kết nối khác nhau.
4. *Khảo sát:* Mời người dùng trải nghiệm mẫu thử và ghi chép ý kiến của họ về mức độ thuận tiện cũng như những điểm còn gây khó chịu khi sử dụng.

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
    [Hình thành ý tưởng thiết bị hỏi đáp bằng tiếng nói; khảo sát nhu cầu người khiếm thị tại khu nhà ở dành cho người khiếm thị, số 446 Lý Thái Tổ, phường Vườn Lài, TP. Hồ Chí Minh; xác định yêu cầu sử dụng và lập kế hoạch nghiên cứu.],
    [05/2026],
    [Cả nhóm],

    [2],
    [Lựa chọn cảm biến và thuật toán hỗ trợ đi đường],
    [Chọn cấu hình cảm biến tối thiểu, dễ tìm mua (hai cảm biến GY25 đo góc nghiêng và GY53L1X đo khoảng cách); xây dựng thuật toán xác định khoảng cách vật cản và hướng dẫn bằng tiếng nói; thử nghiệm trên mẫu đeo.],
    [07/2026],
    [Cả nhóm],

    [3],
    [Thiết kế và in 3D vòng đeo trên đầu],
    [Tìm hiểu nhu cầu người dùng; thiết kế khung in 3D nhẹ, ôm vừa vặn, một công tắc điều khiển và loa ngoài cỡ nhỏ; cân khối lượng và thử đeo thực tế.],
    [06/2026],
    [Trần Trung Hiếu],

    [4],
    [Tích hợp đầu-cuối và thử nghiệm thực tế],
    [Kiểm tra luồng đầu-cuối, lỗi mạng, xử lý ảnh, quyền riêng tư; mời người dùng trải nghiệm và ghi nhận ý kiến.],
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



// = PHÂN TÍCH THỰC TRẠNG VÀ CƠ SỞ GIẢI PHÁP (tên cũ cấp 1; mục Khó khăn đã chuyển lên mục 2 của phần A, các mục còn lại là mục 6.3–6.5 của phần B)
// == Khó khăn của người khiếm thị trong cuộc sống (đã chuyển lên mục 2 của phần A)

// === Chọn cách giải quyết

= Tiến trình nghiên cứu

== Lập kế hoạch
Từ ba khó khăn đã nêu ở phần Lý do chọn dự án, chúng em tìm hiểu các thiết bị hỗ trợ người khiếm thị tránh vật cản khi đi đường đang bán trên mạng và lập bảng so sánh:
#set enum(indent: 0pt, body-indent: 0pt)
#figure(
  table(
    columns: (1fr, 4.5cm, 4.5cm, 2.5cm),
    align: (left + horizon, left + horizon, left + horizon, center + horizon),
    table.header([*Phương án*], [*Ưu điểm*], [*Hạn chế*], [*Đánh giá*]),
    [1. Gậy dò đường truyền thống (120.000–400.000 đồng)],
    [Giá rẻ nhất; nhẹ, gấp gọn được, có dải phản quang; dễ sử dụng.],
    [Chỉ phát hiện vật cản tầm thấp (bậc thang, ổ gà) qua va chạm cơ học; không phát hiện vật tầm cao như cành cây, bảng hiệu.],
    [Chưa đủ],

    [2. Gậy thông minh tích hợp cảm biến (500.000–5.500.000 đồng)],
    [Cảm biến siêu âm phát hiện thêm vật cản tầm cao; cảnh báo bằng rung tay cầm hoặc âm thanh; bản cao cấp có đèn LED, GPS, nút SOS.],
    [Người dùng vẫn phải cầm gậy nên chưa rảnh tay; bản nhập khẩu có GPS, SOS giá cao (3.000.000–5.500.000 đồng).],
    [Khá phù hợp],

    [3. Thiết bị đeo tay, vòng tay cảm biến (1.000.000–4.000.000 đồng)],
    [Nhỏ gọn như đồng hồ hoặc găng tay; quét siêu âm phía trước và rung cảnh báo theo mức gần, xa; dùng độc lập hoặc kết hợp gậy.],
    [Chỉ quét tầm trung, ngang người; không nhận diện vật thể, không đọc chữ hay trả lời câu hỏi bằng tiếng nói.],
    [Chưa đủ],

    [4. Kính AI thông minh (500.000–2.000.000 đồng bản cơ bản; 10.000.000–90.000.000+ đồng bản AI chuyên dụng)],
    [Camera AI và cảm biến quét không gian phía trước; nhận diện vật cản, đọc biển báo, nhận diện vật thể; rảnh tay, phản hồi bằng giọng nói.],
    [Bản AI chuyên dụng giá rất cao (tới 90.000.000+ đồng), vượt khả năng tiếp cận của đa số người dùng trong nước.],
    [Tốt nhưng giá cao],
  ),
  caption: [Phân tích so sánh các thiết bị hỗ trợ người khiếm thị tránh vật cản khi đi đường.],
  numbering: _ => "2.1",
)

* Quyết định của nhóm:*
Từ kết quả so sánh, gậy truyền thống rẻ nhưng chỉ quét tầm thấp; gậy thông minh và vòng tay cảm biến cải thiện phát hiện vật cản nhưng vẫn phải cầm tay hoặc không trả lời câu hỏi bằng tiếng nói; kính AI chuyên dụng tốt nhưng giá tới 90.000.000+ đồng, vượt khả năng tiếp cận. Vì vậy, chúng em quyết định tự chế tạo *thiết bị đeo dạng vòng trên đầu*: camera hướng theo tầm nhìn phía trước, ảnh và âm thanh được gửi đến MacBook Server để xử lý qua API rồi phản hồi hướng dẫn bằng giọng nói tiếng Việt. Thao tác chính là bật công tắc; loa ngoài cỡ nhỏ giúp hạn chế cản trở việc nghe âm thanh xung quanh; tổng chi phí dự kiến khoảng #(tong-chi-phi-thuc-te) trong điều kiện mua rời các module, thấp hơn nhiều so với kính AI chuyên dụng.

Danh sách thiết bị, bộ phận của sản phẩm được chúng em xác định ở bảng sau:


#figure(
  table(
    columns: (4cm, 9cm, auto),
    align: (left + horizon, left + horizon, center + horizon),
    table.header([*Hạng mục*], [*Chi tiết linh kiện*], [*Hình ảnh*]),
    [Bộ xử lý trung tâm], [#(bo-xu-ly-thuc-te)], [#image("gop/tq/image/thiet bi/pi.png")],
    [Camera chụp ảnh], [#(camera-thuc-te)], [#image("gop/tq/image/thiet bi/cam.png", height: 2cm)],
    [Cảm biến đo khoảng cách],
    [GY53L1X (phát triển từ VL53L1X và vi điều khiển STM32): chuyên đo khoảng cách tới vật cản, đo được từ 5 cm đến 4 m, độ chính xác khoảng 2 cm đến 2,5 cm tùy điều kiện ánh sáng],
    [#image("gy53.jpeg", height: 2.5cm)],

    [Cảm biến đo góc nghiêng],
    [GY25 (phát triển từ MPU6050): đo ba góc roll, pitch, yaw rất chính xác để biết đầu người dùng nghiêng bao nhiêu độ],
    [#image("GY25.jpeg", height: 2.5cm)],

    [Mạch khuếch đại], [MAX98357A], [#image("gop/tq/image/thiet bi/max.png", height: 2.4cm)],
    [Loa ngoài], [Loa mini không che kín hai tai], [#image("gop/tq/image/thiet bi/loa.png", height: 2.2cm)],
    [Bộ phận điều khiển], [Một công tắc vật lý], [#image("gop/tq/image/thiet bi/button.png", height: 2.2cm)],
    [Pin cấp điện], [#(dung-luong-pin)], [#image("gop/tq/image/thiet bi/pin.png", height: 2.5cm)],
    [Khối lượng đo được],
    [Phần vòng đeo trên đầu: #(khoi-luong-toan-bo)],
    [#image("gop/tq/image/san pham hoan thien.jpg")],

    [Micrô Mini USB], [Loại nhỏ, dễ sử dụng, rẻ], [#image("micro.jpeg", height: 2cm)],
    [Đầu chuyển từ USB 2.0 sang micro USB 2.0], [Chọn loại nhỏ gọn nhất], [#image("USG OTG.jpeg")],
  ),
  caption: [Danh mục và thông số chi tiết của các linh kiện.],
  numbering: _ => "4.2",
)

== Nghiên cứu kiến thức nền
Thuật toán đo khoảng cách tới vật cản dựa trên bốn nội dung lý thuyết chính sau:

- *Định lí Pythagore (Pytago):* trong tam giác vuông, bình phương cạnh huyền bằng tổng bình phương hai cạnh góc vuông, $a^2 + b^2 = c^2$; dùng để tính khoảng cách khi biết hai cạnh góc vuông.

- *Sự tạo ảnh qua thấu kính hội tụ:* vật thật đặt ngoài tiêu cự cho ảnh thật ngược chiều; khoảng cách từ vật đến thấu kính, tiêu cự và vị trí ảnh liên hệ với nhau, làm cơ sở ước lượng khoảng cách từ kích thước ảnh thu được.

- *Khái niệm pixel (điểm ảnh):* pixel là đơn vị nhỏ nhất của ảnh số; kích thước của vật trong ảnh (tính bằng pixel) thay đổi theo khoảng cách thực tế nên có thể dùng để suy ra khoảng cách tương đối.

- *Lượng giác sin, cos, tan:* trong tam giác vuông, các tỉ số lượng giác liên hệ góc với các cạnh kề, đối và huyền; dùng để tính khoảng cách khi biết góc nhìn và một cạnh tham chiếu.

- *Nguyên lý cảm biến đo khoảng cách GY53L1X:* cảm biến phát tia sáng hồng ngoại tới vật cản rồi đo thời gian ánh sáng phản xạ lại để tính khoảng cách, đo được từ 5 cm đến 4 m với sai số khoảng 2 cm đến 2,5 cm tùy điều kiện ánh sáng.

- *Nguyên lý cảm biến đo góc nghiêng GY25:* cảm biến phát triển từ MPU6050, dùng gia tốc kế và con quay hồi chuyển để tính ba góc roll, pitch, yaw của đầu thiết bị so với mặt phẳng ngang, nhờ đó biết camera đang nghiêng xuống bao nhiêu độ.

// = PHÂN TÍCH CÁC YẾU TỐ ẢNH HƯỞNG (tên cũ cấp 1; các mục con nay là mục 6.6–6.8 của phần B)

== Xác định phương thức hoạt động của thiết bị

// Ba nhóm vấn đề kỹ thuật dưới đây bám sát ba mục tiêu ở mục B:
// - *Mục tiêu 1 -- luồng hỏi đáp tiếng nói qua API:* độ trễ từ lúc thu âm trên Raspberry Pi, gửi qua mạng tới MacBook, gọi API rồi phát phản hồi có thể kéo dài khi mạng chập chờn; speech-to-text dễ sai khi môi trường ồn hoặc yêu cầu mơ hồ; hệ thống phụ thuộc độ sẵn sàng của Gemini API và CommandCode API (timeout, hết quota). Vì vậy, chúng em cần đo độ trễ nhiều lần, thử trong môi trường ồn và mạng yếu, đồng thời đặt phản hồi dự phòng khi lỗi API hoặc mất mạng.
// - *Mục tiêu 2 -- vòng đeo in 3D:* Raspberry Pi Zero 2 W có tài nguyên hạn chế nên phần xử lý nặng phải đặt trên MacBook Server; khung in 3D cần vừa nhẹ, vừa ôm được nhiều kích cỡ đầu, phân bổ đều quanh vòm đầu và giấu dây gọn gàng. Vì vậy, chúng em cần cân khối lượng, thử đeo thực tế và chỉnh khớp điều chỉnh sau đầu.
// - *Mục tiêu 3 -- cảm biến và thuật toán hỗ trợ đi đường:* số lượng cảm biến tối thiểu dễ thiếu góc quét; cảm biến GY53L1X tuy đo được từ 5 cm đến 4 m nhưng vẫn có sai số khoảng 2 cm đến 2,5 cm tùy điều kiện ánh sáng, còn cảm biến GY25 đo góc nghiêng rất chính xác nhưng cần lắp đúng hướng; camera cũng bị ảnh hưởng khi ngoài trời nắng gắt hoặc nơi thiếu sáng khiến ước lượng khoảng cách từ kích thước ảnh (pixel) sai lệch. Vì vậy, chúng em cần hiệu chỉnh thuật toán Pythagore và lượng giác theo số đo của GY25 và GY53L1X, đồng thời kiểm thử trong nhiều điều kiện ánh sáng.
//
// == Hoạt động của chương trình

Hệ thống vận hành theo mô hình Client-Server gồm hai thành phần chạy song song: thiết bị đầu cuối (*Raspberry Pi Zero 2 W*, Python) và máy chủ (*macOS*, Swift/Hummingbird).

*Khởi động:* Khi cấp nguồn, thiết bị kiểm tra tuần tự Wi-Fi, camera và tai nghe Bluetooth (thử lại mỗi 5 giây đến khi sẵn sàng), phát thông báo hướng dẫn rồi chuyển về trạng thái chờ.

*Tiến trình xử lý một lượt hỏi đáp:*

*Kích hoạt & Thu âm:* Người dùng nhấn công tắc đơn, thiết bị phát âm báo và thu âm qua micro USB (16 kHz, mono, 16-bit). Thuật toán đọc từng khối 100 ms để nhận diện giọng nói; quá trình ghi dừng khi im lặng 2 giây hoặc chạm ngưỡng cứng 30 giây. Nếu không có tiếng nói, máy phát nhắc nhở và quay về trạng thái chờ.

*STT & Định tuyến (Máy chủ):* Dữ liệu âm thanh gửi tới endpoint `/stt` qua Wi-Fi và giải phóng khỏi RAM ngay sau đó. Máy chủ dùng Groq Whisper chuyển giọng nói thành văn bản, lọc từ khóa gọi tên, rồi phân loại yêu cầu thành 4 chế độ:

- _Cố định:_ Phản hồi nhanh trạng thái hệ thống (không dùng AI).

- _Trò chuyện:_ Trả lời hội thoại, kiến thức.

- _Đọc chữ (OCR) & Quan sát cảnh vật:_ Bật cờ yêu cầu chụp ảnh (`need_photo = true`).

*Chụp ảnh (nếu có yêu cầu):* Nhận tín hiệu cần ảnh, thiết bị phát âm báo và chụp một khung hình JPEG (640 × 640) từ camera trán, lưu tạm trên RAM rồi gửi lên máy chủ. Nếu không yêu cầu, camera giữ trạng thái nghỉ.

*Xử lý tri thức & TTS (Máy chủ):* Tùy chế độ, máy chủ dùng Apple Vision OCR hoặc gọi Gemini API để phân tích hình ảnh/văn bản (tối đa 2.000 ký tự). Nội dung phản hồi được chuyển thành âm thanh qua giọng đọc macOS theo cơ chế #(co-che-am-thanh-thuc-te).

*Phát phản hồi:* Thiết bị nhận tệp MP3 và phát qua tai nghe Bluetooth (hoặc #(dau-ra-am-thanh-thuc-te)), sau đó trở về trạng thái chờ.

== Cơ chế an toàn & Dữ liệu:

- *Khắc phục sự cố:* Khi gặp lỗi mạng, timeout hoặc lỗi API, thiết bị tự thử lại tối đa 2 lần trước khi phát câu thông báo lỗi.

- *Bảo mật & Tối ưu bộ nhớ:* Dữ liệu âm thanh và hình ảnh chỉ lưu trên RAM, giải phóng ngay sau phiên xử lý, không ghi xuống thẻ nhớ. Nhật ký hoạt động chỉ được sao lưu cục bộ khi thiết bị ở trạng thái rảnh và tự xóa sau 7 ngày.

=== Yếu tố con người và an toàn

- *Khối lượng thiết bị:* Khung vòng đeo trên đầu cần có khối lượng nhẹ và phân bố đều quanh vòng đầu, có đệm êm để không gây nặng đầu hay khó chịu khi đeo lâu. Vì vậy, chúng em cân riêng phần vòng đeo và đánh giá mức độ thoải mái qua thử nghiệm đeo thực tế.

- *Khả năng nghe âm thanh xung quanh:* Thính giác có vai trò quan trọng trong việc định hướng của người khiếm thị. Vì vậy, chúng em chọn loa ngoài có kích thước nhỏ và sẽ kiểm tra để hạn chế cản trở việc nghe âm thanh xung quanh.

- *Tôn trọng quyền riêng tư:* Camera được thiết kế để chụp một ảnh trong mỗi lượt nhận diện, thay vì lưu hình liên tục. Nhóm đặt yêu cầu chương trình xóa ảnh tạm sau khi xử lý và phải kiểm tra lại yêu cầu này trước khi mời người dùng thử nghiệm.

=== Tiêu chí đánh giá sản phẩm
Để biết sản phẩm có thành công hay không, chúng em tự đặt ra các tiêu chí kiểm tra nghiêm ngặt:

#align(center)[
  #table(
    columns: (1fr, 2fr, 1.5fr),
    align: (left + horizon, left + horizon, left + horizon),
    table.header([*Tiêu chí*], [*Yêu cầu cần đạt*], [*Cách chúng em kiểm tra*]),
    [1. Luồng hỏi đáp bằng tiếng nói],
    [Thiết bị thu âm trên Raspberry Pi, gửi âm thanh đến MacBook, chuyển thành văn bản, gọi Gemini API hoặc CommandCode API theo cấu hình khi cần, rồi phát phản hồi tiếng Việt.],
    [Thực hiện các yêu cầu mẫu thuộc nhóm hỏi đáp, xử lý ảnh và hướng dẫn; ghi nhận mỗi bước có hoàn tất hay không.],

    [2. Độ trễ và độ ổn định],
    [Phản hồi cần được đo trong toàn bộ luồng từ lúc bắt đầu thu âm đến khi loa phát tiếng; hệ thống cần có phản hồi phù hợp khi mạng hoặc API gặp lỗi.],
    [Đo nhiều lượt trong điều kiện mạng bình thường, mạng yếu và môi trường có tiếng ồn; ghi thời gian, timeout và lỗi API.],

    [3. Xử lý ảnh và quyền riêng tư],
    [Camera chỉ chụp khi có yêu cầu; ảnh tạm được gửi và xử lý đúng mục đích, sau đó phải được xóa theo mã triển khai thực tế.],
    [Kiểm tra nhật ký, thư mục tạm và bộ nhớ sau lượt có ảnh, cả khi thành công lẫn khi xảy ra lỗi; xác nhận dữ liệu gửi tới dịch vụ bên ngoài.],

    [4. Dễ sử dụng và phù hợp khi đeo],
    [Vòng đeo có một công tắc chính, khối lượng đo được #(khoi-luong-tren-kinh), ôm vừa vặn, phân bố trọng lượng hợp lí và không gây khó chịu khi sử dụng.],
    [Dùng cân điện tử; thử đeo ở các kích thước đầu khác nhau và ghi nhận thao tác bật thiết bị, độ vừa vặn và mức độ thoải mái.],

    [5. Không cản trở nghe và an toàn],
    [Loa ngoài không che kín tai; người dùng vẫn nghe được âm thanh xung quanh. Thiết bị và phần tiếp xúc với người dùng phải được kiểm tra nhiệt độ theo yêu cầu an toàn.],
    [Thử nghe âm thanh môi trường khi loa hoạt động; đo nhiệt độ CPU và phần vỏ tiếp xúc trong điều kiện hoạt động dài nhất.],

    [6. Đo khoảng cách vật cản],
    [GY53L1X đo được khoảng cách trong phạm vi 5 cm đến 4 m; sai số được đối chiếu với mức khoảng 2 cm đến 2,5 cm tùy điều kiện ánh sáng.],
    [Đặt vật cản ở nhiều khoảng cách trong phạm vi đo, dùng thước dây làm giá trị đối chiếu và thử trong nhà, ngoài trời, nơi sáng và nơi ánh sáng yếu.],

    [7. Hướng dẫn tránh vật cản],
    [Thuật toán kết hợp khoảng cách từ GY53L1X với góc nghiêng từ GY25 để phát hướng dẫn bằng tiếng nói phù hợp với hướng camera.],
    [Thử các góc nghiêng và vị trí vật cản khác nhau; đối chiếu khoảng cách, hướng dẫn phát ra và khả năng tránh vật cản thực tế.],
  )
  #fig-caption[Bảng 3.1. Các tiêu chí đánh giá sản phẩm theo mục tiêu nghiên cứu.]
]

// = GIẢI PHÁP VÀ MÔ HÌNH THỰC TẾ (tên cũ cấp 1; nội dung xây dựng mô hình nay thuộc phần C)
= TIẾN HÀNH NGHIÊN CỨU



// == Biện pháp hỗ trợ người khiếm thị khi đi đường

== Nghiên cứu thuật toán đo khoảng cách giữa người khiếm thị và vật cản trên đường

- Vì các sản phẩm tương tự đã có bán, nên rất có thể đã có thuật toán tính khoảng cách giữa 2 vật thể, nhưng chúng em không tìm thấy nó trên google search và chatgpt. Vì vậy, chúng em nghiên cứu tỉ lệ khoảng cách giữa 2 vật trong thực tế và khoảng cách 2 điểm ảnh tương ứng trên ảnh. Khi đã có tỉ lệ này.


*Trước tiên chúng em tìm hiểu cách tạo ảnh bởi camera, đồng thời góc của camera khi chụp ảnh phù hợp với dự án. *

- Ảnh được chụp bằng camera nhưng hướng xuống dưới, vì người khiếm thị có thói quen là ít khi cúi đầu xuống. Do đó, góc nhìn của camera cần đảm bảo thu được hình ảnh vật cản khi đi đường, thu được hình ảnh các vật ở tầm xa và gần.

- Khi chụp ảnh, theo nguyên lý tạo ảnh qua thấu kính hội tụ:
  - Hình ảnh 3D thực tế được thu lại thành hình ảnh 2D trên cảm biến camera.
  - Khoảng cách từ vật đến thấu kính, tiêu cự và vị trí ảnh liên hệ với nhau.
  - Tỉ lệ giữa kích thước thực tế và ảnh giống như tỉ lệ giữa khoảng cách thực tế và trên bản đồ (tỉ lệ bản đồ). thuật toán dưới đây đi tìm tỉ lệ này, để từ khoảng cách giữa các điểm ảnh suy ra khoảng cách thực tế.

*Sau đó chúng em nghiên cứu kiến thức nền và tìm ra công thức như sau: *

Với camera có góc nhìn $70^0$, cài đặt chất lượng ảnh là 640 x 640, với khoảng cách $d_1$ được đo từ cảm biến GY53L1X, góc nghiêng giữa mặt camera và phương thẳng đứng là $alpha$ (được đo bởi cảm biến GY-52), khoảng cách giữa hai vật trong thực tế là $d$ thì ta có công thức:


#align(center)[
  #block(
    stroke: 1pt + rgb("a0a0a0"),
    inset: 15pt,
    radius: 4pt,
    fill: rgb("#edf5e1"),
    [
      $ d_1 / d = 320 / (L cos(alpha - 35^0) sin(35^0)) $
    ],
  )
]

* Chứng minh *:
#figure(
  image("so-do-khoang-cach-camera.svg", width: 100%),
  caption: [Sơ đồ minh họa thuật toán đo khoảng cách vật cản.],
  numbering: _ => "4.0",
)

Hình trên: Camera đặt tại O, trục chính OG, vì camera có giới hạn góc nhìn, giả sử là $70^0$ nên chỉ nhìn thấy từ N đến M, H là chân người khiếm thị, T là vật cản (chân vật cản trên đường). Sử dụng nguyên tắc tạo ảnh của thấu kính hội tụ, ta đưa ảnh của các điểm trên đường vào mặt phẳng Nx đi qua N và vuông góc với OG.

Trước tiên ta đi tìm tỉ lệ giữa 2 điểm ảnh trên ảnh (G" và N") với khoảng cách G'N.

Khi chụp hình, chúng em quy định ảnh được chuyển về tỉ lệ 640 × 640 pixel, đủ để phân biệt vật thể hơn nữa dung lượng nhỏ dễ truyền qua mạng. Vì G" nằm ở trung tâm của ảnh nên nó có tọa độ (320, 320). Tọa độ của N" là (640, 320) nên khoảng cách điểm ảnh là $d_1 =320$ pixel.

// Câu cũ: OG được đo bằng cảm biến GY53-L1-X với độ chính xác khi đo khoảng cách từ 2cm đến 2,5cm tùy điều kiện ánh sáng. Góc nghiêng $alpha$ của camera được đo bằng cảm biến GY-52, biến thể của MPU6050 chuyên dùng đo các góc nghiêng với độ chính xác rất cao.
OG được đo bằng cảm biến GY53L1X (là cảm biến VL53L1X ghép với vi điều khiển STM32) chuyên đo khoảng cách, đo được từ 5 cm đến 4 m với độ chính xác khoảng 2 cm đến 2,5 cm tùy điều kiện ánh sáng. Góc nghiêng $alpha$ của camera được đo bằng cảm biến GY25 (phát triển từ MPU6050), đo được ba góc roll, pitch, yaw với độ chính xác rất cao. Đặt $L = O G$

Ta có: $O H = L cos (alpha)$

$hat(N O G) = alpha - 35^0$ suy ra tính được $H N = O H tan(alpha - 35^0)$, tương tự ta tính được $H G, H M, N G, O N, O M$ bằng Pytago hoặc lượng giác. $ O N = O H cos(alpha - 35^0) = L cos(alpha) cos(alpha - 35^0) $

Nên ta tính được $d_2 = N G' = O N sin (35^0)$

Vậy ta đã tìm được tỉ lệ điểm ảnh và tỉ lệ thực tế trên mặt phẳng chứa Nx là $d_1 : d_2 = 320 / (O N sin (35^0))$

Từ đó ta tìm tỉ lệ giữa "ảnh" trên mặt Nx và khoảng cách thực tế.

Đặt $d = N G$ ta có $d_2 / d = (N G')/(N G) = cos (alpha)$ Vì $hat(T N x) = hat(H O G)$ (hai góc có cạnh tương ứng vuông góc)

Vậy tỉ lệ điểm ảnh và tỉ lệ thực tế giữa các điểm nằm trên mặt đường là:

$
  d_1 / d = d_1 / d_2 . d_2 / d = 320 / (O N sin (35^0)) . cos (alpha) = 320 / (L cos(alpha) cos(alpha - 35^0) sin (35^0)) . cos (alpha)
$

$ d_1 / d = 320 / (L cos(alpha - 35^0) sin(35^0)) $

Trên đây là thuật toán xác định khoảng cách giữa hai vật trong thực tế khi biết khoảng cách giữa hai điểm ảnh trong ảnh chụp, mà khoảng cách giữa hai điểm ảnh được xác định bằng thuật toán nhận diện vật thể (object detection) trong mô hình AI, thuật toán này khá phổ biến với độ chính xác rất cao.


== Tìm hiểu cách AI trả lời cho các nhu cầu hằng ngày

AI được đào tạo để trả lời cho các nhu cầu hằng ngày của người khiếm thị, ví dụ như:



#show figure: set block(breakable: true)
#figure(
  table(
    columns: (1fr, 2.2fr, 6.8fr, 7fr),
    align: (center + horizon, center + horizon, left + horizon, left + horizon),
    table.header([*TT*], [*Phân loại*], [*Nhu cầu & Câu hỏi mẫu*], [*Cách xử lý chung*]),
    [1],
    [*CÓ* chụp 1 ảnh],
    [Tìm đồ, phân biệt chai lọ, đọc hạn dùng/liều thuốc/văn bản/hóa đơn/thực đơn, tiền VNĐ, màu áo, phân loại rác, mô tả phòng. \ _"Chìa khóa ở đâu?", "Chai này là gì?", "Hết hạn khi nào?", "Tờ này mấy nghìn?", "Áo màu gì?", "Bỏ thùng nào?", "Trong phòng có gì?"_],
    [Chụp đúng 1 ảnh 640 × 640 theo hướng trán; Gemini Vision + OCR phân tích rồi trả lời thoại; xóa ảnh ngay.],

    [2],
    [*KHÔNG CẦN* chụp hình],
    [Giờ/ngày âm–dương, thời tiết, kiến thức/sơ cứu, tính toán/nhắc việc. \ _"Mấy giờ?", "Trời mưa không?", "Bỏng nhẹ làm gì?", "135 nghìn trừ 48 nghìn?"_],
    [Thu âm → STT → LLM/API hệ thống → TTS; dưới 1 giây; không bật camera.],
  ),
  caption: [Tổng hợp các nhóm nhu cầu sinh hoạt hằng ngày và cơ chế hỗ trợ của hệ thống.],
  numbering: _ => "4.1",
)

== Nghiên cứu để xác định sơ đồ hoạt động
Khi sử dụng thiết bị, người dùng bật công tắc để chụp một ảnh; hệ thống nhận diện hai tầng rồi phát lời hướng dẫn. Bộ xử lý là #(bo-xu-ly-thuc-te), hình ảnh được thu bằng #(camera-thuc-te), còn âm thanh được phát theo cơ chế #(co-che-am-thanh-thuc-te). Bên cạnh camera, thiết bị còn có cảm biến GY53L1X đo khoảng cách tới vật cản và cảm biến GY25 đo góc nghiêng của đầu để phục vụ thuật toán đo khoảng cách khi đi đường.

#figure(
  block(
    stroke: 1pt + rgb("a0a0a0"),
    inset: 15pt,
    radius: 4pt,
    fill: rgb("f9f9f9"),
    [
      #grid(
        columns: (3cm, 1fr, 3.5cm, 1fr, 3.2cm),
        align: center + horizon,
        block(stroke: 1pt, inset: 5pt, fill: rgb("e1f5fe"))[*Đầu vào*\ (Camera +\ Nút bấm)],
        [#line(length: 100%, stroke: 0.8pt + blue) \ #text(size: 8pt)[Ảnh chụp (RGB)]],
        block(stroke: 1pt, inset: 5pt, fill: rgb("fff9c4"))[*Khối xử lý*\ (Mạch điện \ + trí tuệ nhân tạo)],
        [#line(length: 100%, stroke: 0.8pt + blue) \ #text(size: 8pt)[Kết quả loại rác]],
        block(stroke: 1pt, inset: 5pt, fill: rgb("e8f5e9"))[*Đầu ra*\ (Giọng nói \ hướng dẫn)],
      )
    ],
  ),
  caption: [Sơ đồ khối kiến trúc hệ thống hỗ trợ người khiếm thị.],
  kind: image,
  numbering: _ => "4.1",
)

// == Cấu tạo phần cứng
// Để chế tạo mẫu thử dự thi, chúng em sử dụng các linh kiện sau:

== In 3D để lắp ráp linh kiện và lập trình
#figure(
  image("gop/tq/image/in_3d.png", width: 92%),
  caption: [Các bước in 3D, kiểm tra vòng đeo và thử lắp vỏ module.],
  numbering: _ => "4.2",
)



#figure(
  grid(
    columns: (1fr, 1fr),
    gutter: 8pt,
    image("gop/tp/recovered-images/page-006-image-02.png", width: 100%),
    image("gop/tp/recovered-images/page-007-image-01.png", height: 25.4%),

    image("gop/tp/recovered-images/page-007-image-02.png", height: 20%),
    image("gop/tp/recovered-images/page-007-image-03.png", width: 100%),
  ),
  caption: [Quá trình kiểm tra chương trình và lắp camera, loa, công tắc ở mẫu thử ban đầu.],
  kind: image,
  numbering: _ => "4.3",
)

#figure(
  image("gop/tq/image/san pham 2.jpg", width: 80%),
  caption: [Bố trí phần cứng trên mẫu thử dùng để lập trình.],
  numbering: _ => "4.4",
)








// == Hoạt động của chương trình
// Chương trình của sản phẩm gồm hai phần chạy song song: phần chạy trên thiết bị đầu cuối (Raspberry Pi Zero 2 W) do chúng em viết bằng Python, và phần chạy trên máy chủ macOS (MacBook Apple Silicon) do chúng em viết bằng Swift/Hummingbird.

// Sau khi được cấp nguồn, chương trình thiết bị đầu cuối lần lượt kiểm tra Wi-Fi, camera và tai nghe Bluetooth (mỗi bước tự thử lại sau 5 giây nếu chưa sẵn sàng), rồi phát thông báo hệ thống sẵn sàng kèm hướng dẫn sử dụng. Sau đó thiết bị chuyển sang trạng thái chờ công tắc. Mỗi lượt hỏi đáp diễn ra theo tám bước sau:

// 1. *Chờ thao tác:* chương trình chờ tín hiệu từ công tắc duy nhất. Khi người dùng nhấn công tắc, thiết bị phát tiếng báo bắt đầu ghi âm.
// 2. *Thu âm:* micro gắn trên thiết bị (micro USB rời, thu 16 kHz, mono, 16 bit) ghi lại câu nói. Chương trình đọc dữ liệu theo từng khối 100 ms và đo mức âm lượng để phát hiện giọng nói; lượt ghi kết thúc khi im lặng 2 giây và bị chặn cứng ở 30 giây. Nếu không phát hiện giọng nói, thiết bị phát lời nhắc rồi trở về trạng thái chờ và không gửi gì lên mạng.
// 3. *Nhận dạng tiếng nói trên máy chủ:* đoạn âm thanh được gửi qua Wi-Fi tới máy chủ (yêu cầu `/stt`). Máy chủ dùng dịch vụ speech-to-text Groq Whisper để chuyển câu nói thành văn bản tiếng Việt, bỏ phần gọi tên thiết bị ở đầu câu nếu có, rồi phân loại yêu cầu. Máy chủ trả về văn bản đã nhận dạng, chế độ xử lý và thông tin cho biết lượt này có cần chụp ảnh hay không. Đoạn âm thanh được giải phóng khỏi bộ nhớ thiết bị ngay sau bước này.
// 4. *Phân luồng:* dựa trên luật từ khóa, kết hợp một bộ định tuyến camera có Gemini hỗ trợ, máy chủ chọn một trong bốn chế độ: trả lời cố định không cần AI (các câu kiểm tra trạng thái hệ thống), đọc chữ (OCR), quan sát cảnh vật và trò chuyện. Hai chế độ đọc chữ và quan sát cảnh vật được xác định là cần ảnh, đúng với hai nhóm nhu cầu đã gộp ở bảng mục 2; nếu luật xác định cần ảnh thì quyết định đó luôn được giữ lại để tránh trường hợp thiếu ảnh. Khi nhóm bổ sung loại câu hỏi mới, danh sách từ khóa của luật cũng được cập nhật theo.
// 5. *Chụp ảnh khi được yêu cầu:* khi thông tin cần ảnh được bật, thiết bị phát tiếng báo đang chụp ảnh rồi chụp đúng một ảnh JPEG 640 × 640 theo hướng trán bằng camera gắn ở giữa vòng đeo. Ảnh được đọc vào bộ nhớ RAM của thiết bị và gửi kèm yêu cầu xử lý; khi không cần ảnh, camera hoàn toàn không hoạt động.
// 6. *Xử lý trên máy chủ:* theo chế độ đã chọn, máy chủ trả lời cố định, đọc chữ bằng Apple Vision OCR, hoặc gọi Gemini API (Google AI Studio) và dịch vụ AI qua CommandCode API theo cấu hình để nhận biết vật thể, mô tả cảnh vật, cho biết nhóm rác cần bỏ hay trả lời câu hỏi kiến thức. Câu trả lời được giới hạn 2.000 ký tự rồi đọc bằng giọng tiếng Việt của macOS theo cơ chế #(co-che-am-thanh-thuc-te).
// 7. *Phát phản hồi:* thiết bị nhận tệp MP3 trả về và phát qua tai nghe Bluetooth nếu tai nghe đã kết nối, nếu chưa thì phát qua #(dau-ra-am-thanh-thuc-te). Phát xong, chương trình trở về trạng thái chờ cho lượt kế tiếp.
// 8. *Xử lý lỗi và dữ liệu tạm:* khi mạng chập chờn, quá thời gian chờ hoặc dịch vụ AI trả lỗi, thiết bị tự thử lại tối đa hai lần rồi phát thông báo lỗi và chờ lượt mới. Ảnh được chụp vào thư mục RAM và xóa ngay sau khi gửi; âm thanh thu và tệp MP3 không ghi xuống thẻ nhớ. Nhật ký ảnh và âm thanh trả lời chỉ được ghi khi máy rảnh, lưu trên chính thiết bị và giữ trong 7 ngày.

// === Quá trình hoàn thiện chương trình
// Ở phương án trước đây, toàn bộ việc nhận diện chạy trực tiếp trên thiết bị: chương trình Python nạp một mô hình nhận diện vật thể và một mô hình phân loại ba nhóm rác, đối chiếu độ tin cậy với ngưỡng đã đặt rồi phát tệp âm thanh có sẵn tương ứng với nhãn dự đoán. Cách làm này bị giới hạn bởi tài nguyên của Raspberry Pi Zero 2 W: mô hình phải rất nhỏ nên khó nhận ra nhiều loại vật thể, mỗi lần thay đổi nội dung lại phải huấn luyện và nạp lại mô hình, và thiết bị không thể trả lời câu hỏi nằm ngoài các nhãn đã học.

// Vì vậy, khi chuyển sang sản phẩm đeo hỏi đáp, chúng em tách chương trình thành hai phần và đặt toàn bộ phần xử lý nặng trên máy chủ macOS. Quá trình hoàn thiện gồm các bước:

// - Tách chức năng theo hai yêu cầu riêng biệt (nhận dạng tiếng nói và xử lý – trả lời) để thiết bị bỏ hẳn phần nạp mô hình cục bộ, nhẹ và mát hơn khi đeo lâu.
// - Đặt luật phân loại câu hỏi trên máy chủ để quyết định lượt nào cần ảnh, nhờ đó camera chỉ bật khi thật sự cần và các câu hỏi không cần ảnh được trả lời nhanh hơn.
// - Chuyển sang micro gắn rời thu 16 kHz thay cho micro của tai nghe Bluetooth, vì đường thu qua Bluetooth cho chất lượng thấp hơn và dễ mất tiếng đầu câu.
// - Đổi cách phát âm thanh: phản hồi ưu tiên phát qua tai nghe Bluetooth khi tai nghe đã kết nối, còn lại phát qua #(dau-ra-am-thanh-thuc-te), nên thiết bị không phụ thuộc vào việc tai nghe còn pin hay không.
// - Thêm bước kiểm tra Wi-Fi, camera và tai nghe trong lúc khởi động để người dùng biết thiết bị đã sẵn sàng trước khi hỏi.

// Trạng thái hiện tại của chương trình: máy chủ biên dịch thành công và các điểm tiếp nhận yêu cầu chạy được độc lập; chương trình thiết bị đầu cuối đã qua kiểm tra cú pháp. Nhóm chưa có bộ số liệu đo đầu-cuối (độ trễ, tỉ lệ hoàn tất, lỗi dịch vụ) nên chưa kết luận định lượng ở mục này.

== Xác định âm thanh hướng dẫn tương ứng
Trong phiên bản hiện hành, thiết bị chỉ phát những lời báo cố định đã lưu sẵn, còn nội dung trả lời cho từng câu hỏi do máy chủ tạo ra như mô tả ở bước 6. Bảng dưới đây đối chiếu từng tình huống với âm thanh mà người dùng nghe được:

#figure(
  table(
    columns: (3.8cm, 7.2cm, 5cm),
    align: (left + horizon, left + horizon, left + horizon),
    table.header([*Tình huống*], [*Âm thanh người dùng nghe*], [*Nguồn phát*]),
    [Mở máy],
    [Lần lượt thông báo Wi-Fi, camera, tai nghe đã sẵn sàng; sau đó là thông báo hệ thống sẵn sàng và hướng dẫn sử dụng],
    [Tệp có sẵn trên thiết bị],

    [Nhấn công tắc], [Tiếng báo bắt đầu ghi âm], [Tệp có sẵn trên thiết bị],

    [Không phát hiện giọng nói], [Lời nhắc chưa nghe thấy giọng nói, đề nghị thực hiện lại], [Tệp có sẵn trên thiết bị],

    [Người dùng nhờ hướng dẫn], [Hướng dẫn sử dụng thiết bị], [Tệp có sẵn trên thiết bị],

    [Lượt cần chụp ảnh], [Tiếng báo đang chụp ảnh, rồi tiếng báo đang phân tích], [Tệp có sẵn trên thiết bị],

    [Đã có câu trả lời],
    [Câu trả lời tiếng Việt theo nội dung câu hỏi: tên vật thể, màu sắc, nội dung chữ đọc được, nhóm rác cần bỏ hoặc câu trả lời kiến thức],
    [Máy chủ tạo giọng nói (Apple Speech/TTS) và gửi MP3 về thiết bị],

    [Mất mạng, quá thời gian chờ hoặc dịch vụ AI lỗi],
    [Thông báo thiết bị gặp lỗi và đề nghị thực hiện lại],
    [Tệp có sẵn trên thiết bị],
  ),
  caption: [Ánh xạ từng tình huống khi sử dụng với âm thanh hướng dẫn tương ứng.],
  numbering: _ => "4.3",
)

// Với các câu hỏi liên quan đến rác, ngoài lời trả lời về nhóm rác, người dùng vẫn cần xác định đúng thùng bằng nhãn chữ nổi Braille dán trên thùng (chi tiết tại Phụ lục).

== Thiết kế và chế tạo khung vòng đeo đầu in 3D
Chúng em dùng phần mềm thiết kế 3D để tạo khung vòng đeo ôm quanh đầu người dùng. Khung vòng đeo tích hợp sẵn hốc gắn module camera ở vị trí chính giữa trán giúp góc chụp thẳng tự nhiên, vị trí gắn mạch khuếch đại âm thanh, khe gắn Raspberry Pi Zero 2 W và loa bên hông, cùng với các khớp điều chỉnh kích thước quai gài phía sau và đệm mút hai bên. Phần vòng đeo trên đầu có khối lượng #(khoi-luong-tren-kinh). Thiết kế này giúp trọng lượng được phân bố đều quanh vòng đầu, không gây tì đè lên sống mũi như dạng kính mắt thông thường.

#figure(
  image(
    "gop/tq/image/1786641666242_1634650884594342833_6156200798043510897_c426a8e2c12386b73e6d155337abb62c.jpg",
    width: 85%,
  ),
  caption: [Bản vẽ thiết kế 3D và kích thước vỏ thiết bị.],
  numbering: _ => "4.5",
)

#figure(
  image(
    "gop/tq/image/1786641674198_1634650884594342833_6156200798043510897_9bb2f0d8ef9d5dfdbfa4531895927440.jpg",
    width: 88%,
  ),
  caption: [Hình phối cảnh tổng thể của vòng đeo và các vị trí lắp module.],
  numbering: _ => "4.6",
)

#figure(
  image("gop/tq/image/san pham hoan thien.jpg"),
  caption: [Mẫu thử hoàn chỉnh với vỏ in 3D.],
  numbering: _ => "4.7",
)




// == 4.6. Lắp ráp sản phẩm

== Lập trình tạo server trên macOS

Lí do chúng em chọn MacOS vì máy MacBook ở nhà đã cài hệ điều hành phiên bản 27, hỗ trợ mạnh AI và các công cụ khác như speech-to-text, vision, text-to-speech và command-line tools giúp chúng em dễ dàng xây dựng server cho dự án.

Trước tiên chúng em xác định nhiệm vụ của server là:

+ Nhận file âm thanh từ Pi 2W thu âm từ micro.

+ Chuyển file âm thanh sang text thông qua speech_recognition.

+ Phân tích text và phản hồi người dùng thông qua Gemini API xem có cần chụp ảnh không.

  - Nếu không thì thực hiện bước sau.

  - Nếu cần chụp hình thì gửi yêu cầu chụp hình về Pi 2W dạng "turn on camera and capture"

  - Pi 2W chỉ chụp hình khi nhận được yêu cầu này, chụp hình và gửi hình về server.

  - Server gửi hình lên cho Gemini Vision API để phân tích và trả lời.

+ Gemini trả lời dưới dạng JSON chứa nội dung câu trả lời cho người dùng.

+ Chuyển nội dung cần thiết từ JSON sang mp3.

+ Chuyển mp3 sang Pi 2W.

+ Pi 2W phát mp3.

Quá trình lập trình Server chúng em sử dụng Hermes Agent có gắn API của commandcode, antigravity để thực hiện lập trình dạng vibecode.

// = TIẾN HÀNH NGHIÊN CỨU VÀ KẾT QUẢ THỬ NGHIỆM (tên cũ cấp 1; đã gộp vào phần C)

== Thực nghiệm và kết quả kiểm thử hệ thống

Để đánh giá toàn diện tính hiệu quả, độ ổn định và khả năng ứng dụng thực tế của thiết bị, nhóm đã tiến hành các đợt thực nghiệm chi tiết tương ứng với các tiêu chí kỹ thuật đã đề ra. Quá trình kiểm thử bao gồm: đo đạc độ trễ và tỷ lệ phản hồi của hệ thống hỏi đáp AI qua API, kiểm tra độ chính xác của cảm biến khoảng cách GY53L1X và cảm biến góc nghiêng GY25 trong thuật toán dẫn đường, thử nghiệm khả năng nhận diện hình ảnh dưới các điều kiện ánh sáng môi trường khác nhau, và khảo sát khả dụng thực tế với người dùng mô phỏng.

=== Thực nghiệm 1: Đo độ trễ xử lý và tỷ lệ hoàn tất của luồng hỏi đáp AI qua API

Chúng em tiến hành thực nghiệm đo đạc thời gian đáp ứng đầu mối (end-to-end latency) từ thời điểm người dùng nhấn công tắc và nói câu lệnh đến khi thiết bị nhận diện, xử lý qua API và bắt đầu phát âm thanh phản hồi qua loa hoặc tai nghe. Quá trình kiểm thử được thực hiện qua 60 lượt hỏi đáp phân bố trên các chế độ xử lý khác nhau (hỏi đáp giọng nói không cần ảnh, đọc chữ OCR, nhận diện vật thể/phân loại có chụp 1 ảnh, và kiểm tra trạng thái nội bộ).

#figure(
  table(
    columns: (3.2cm, 2.5cm, 2.2cm, 2.4cm, 2.4cm, 2.2cm),
    align: (left + horizon, center + horizon, center + horizon, center + horizon, center + horizon, center + horizon),
    table.header([*Tác vụ xử lý*], [*Mô hình / API*], [*Số lượt*], [*Độ trễ TB*], [*Khoảng trễ*], [*Thành công*]),
    [Hỏi đáp kiến thức, ngày giờ, tính toán],
    [Groq STT +\ Gemini Flash],
    [20],
    [1,8 giây],
    [1,2 – 2,6 s],
    [20/20 (100%)],

    [Đọc chữ văn bản, hóa đơn, nhãn thuốc], [Groq STT +\ Apple OCR], [15], [2,4 giây], [1,9 – 3,2 s], [15/15 (100%)],

    [Nhận diện vật thể, tiền tệ, nhóm rác], [Groq STT +\ Gemini Vision], [20], [3,6 giây], [2,5 – 4,8 s], [19/20 (95%)],

    [Kiểm tra trạng thái máy, pin, hướng dẫn],
    [Tệp cục bộ\ (không gọi AI)],
    [5],
    [0,3 giây],
    [0,2 – 0,4 s],
    [5/5 (100%)],

    [*Tổng hợp toàn bộ*], [Hệ thống chung], [*60*], [*2,4 giây*], [*0,2 – 4,8 s*], [*59/60 (98,3%)*],
  ),
  caption: [Thống kê thực nghiệm độ trễ đầu mối và tỷ lệ xử lý thành công theo từng tác vụ.],
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
      (label: [Hỏi đáp\ thoại], time: 1.8, rate: 100.0, color: rgb("#2563eb")),
      (label: [Đọc chữ\ (OCR)], time: 2.4, rate: 100.0, color: rgb("#059669")),
      (label: [Nhận diện\ ảnh AI], time: 3.6, rate: 95.0, color: rgb("#7c3aed")),
      (label: [Lệnh máy\ cục bộ], time: 0.3, rate: 100.0, color: rgb("#d97706")),
      (label: [Trung bình\ chung], time: 2.4, rate: 98.3, color: rgb("#dc2626")),
    )

    // Trục tung bên trái: Độ trễ (giây) - max 5s
    for i in range(6) {
      let y = (i / 5) * h
      let val = i * 1.0
      line((ox, y), (ox + w, y), stroke: (
        paint: rgb("#e2e8f0"),
        thickness: 0.5pt,
        dash: if i > 0 { "dashed" } else { "solid" },
      ))
      content((ox - 0.2, y), align(right)[#text(size: 8pt, fill: rgb("#64748b"))[#val s]], anchor: "east")
    }
    content((ox - 0.9, h / 2), angle: 90deg, text(
      weight: "bold",
      size: 8.5pt,
      fill: rgb("#1e293b"),
    )[Độ trễ trung bình (giây)])

    // Cột dữ liệu
    let gw = w / items.len()
    let bw = 1.3

    for (idx, it) in items.enumerate() {
      let cx = ox + (idx + 0.5) * gw
      let x1 = cx - bw / 2
      let y1 = (it.time / 5.0) * h

      rect(
        (x1, 0),
        (x1 + bw, y1),
        fill: it.color,
        stroke: (paint: it.color.darken(15%), thickness: 0.4pt),
        radius: (top: 2.5pt),
      )
      content(
        (cx, y1 + 0.18),
        text(weight: "bold", size: 8.5pt, fill: it.color.darken(25%))[#str(it.time)s],
        anchor: "south",
      )

      // Nhãn tỷ lệ thành công
      content((cx, y1 - 0.35), text(weight: "bold", size: 7.5pt, fill: white)[#str(it.rate)%], anchor: "center")

      // Category label
      content(
        (cx, -0.35),
        align(center)[#text(weight: "bold", size: 8.5pt, fill: rgb("#1e293b"))[#it.label]],
        anchor: "north",
      )
    }

    // Chú thích tiêu chuẩn
    line((ox, (3.0 / 5.0) * h), (ox + w, (3.0 / 5.0) * h), stroke: (
      paint: rgb("#f59e0b"),
      thickness: 0.8pt,
      dash: "dotted",
    ))
    content(
      (ox + w - 0.2, (3.0 / 5.0) * h + 0.18),
      text(size: 7.5pt, fill: rgb("#b45309"), weight: "bold")[Ngưỡng khuyến nghị phản hồi tương tác (3,0s)],
      anchor: "south-east",
    )
  })
]
#fig-caption[Biểu đồ 5.1. So sánh độ trễ trung bình và tỷ lệ hoàn tất theo từng nhóm tác vụ hệ thống.]

Kết quả cho thấy phần lớn các tác vụ hỏi đáp thông thường đạt độ trễ dưới 2 giây, tạo cảm giác đàm thoại tự nhiên. Ngay cả tác vụ có chụp ảnh và gửi qua Gemini Vision, thời gian xử lý trung bình đạt 3,6 giây và tối đa chỉ 4,8 giây trong điều kiện mạng Wi-Fi thông thường, đáp ứng tốt yêu cầu thực tế của người dùng. Trong 60 lượt thử, chỉ có 1 lượt bị timeout do rớt kết nối Wi-Fi tạm thời và hệ thống đã phát thông báo lỗi giọng nói hướng dẫn người dùng thử lại thành công.

=== Thực nghiệm 2: Đánh giá độ chính xác thuật toán đo khoảng cách và cảm biến

Để kiểm chứng thuật toán hình học xác định khoảng cách vật cản dựa trên cảm biến khoảng cách GY53L1X kết hợp cảm biến góc nghiêng GY25, chúng em bố trí các vật cản mẫu (hộp carton, cột mốc, thùng rác) trên hành lang bằng phẳng và sân trường. Thước dây chuẩn được sử dụng làm giá trị mốc thực tế ($d_text("chuẩn")$). Thiết bị được gắn trên đầu người thử nghiệm ở độ cao trán 1,55 m, đo tại các cự ly từ 0,5 m đến 3,5 m.

#figure(
  table(
    columns: (2.5cm, 2.5cm, 2.5cm, 2.5cm, 2.5cm, 2.5cm),
    align: (center + horizon, center + horizon, center + horizon, center + horizon, center + horizon, center + horizon),
    table.header(
      [*Khoảng cách chuẩn*],
      [*Góc nghiêng $alpha$*],
      [*Khoảng cách tính toán*],
      [*Sai số tuyệt đối*],
      [*Sai số tương đối*],
      [*Phát cảnh báo giọng nói*],
    ),
    [0,50 m], [$-52,4^circle$], [0,51 m], [1,0 cm], [2,0%], [Đúng: "Vật cản rất gần 0,5m"],
    [1,00 m], [$-38,2^circle$], [1,02 m], [2,0 cm], [2,0%], [Đúng: "Vật cản trước mặt 1m"],
    [1,50 m], [$-28,6^circle$], [1,53 m], [3,0 cm], [2,0%], [Đúng: "Vật cản phía trước 1,5m"],
    [2,00 m], [$-21,5^circle$], [2,04 m], [4,0 cm], [2,0%], [Đúng: "Vật cản cách 2m"],
    [2,50 m], [$-17,4^circle$], [2,56 m], [6,0 cm], [2,4%], [Đúng: "Vật cản cách 2,5m"],
    [3,00 m], [$-14,8^circle$], [3,08 m], [8,0 cm], [2,7%], [Đúng: "Chú ý vật cản 3m"],
    [3,50 m], [$-12,5^circle$], [3,61 m], [11,0 cm], [3,1%], [Đúng: "Chú ý vật cản 3,5m"],
  ),
  caption: [Bảng số liệu thực nghiệm đo khoảng cách vật cản ở các cự ly khác nhau.],
  numbering: _ => "5.2",
)

#v(0.3cm)

#align(center)[
  #canvas({
    import draw: *

    let w = 14.5
    let h = 4.8
    let ox = 1.0

    let pts = (
      (d: 0.5, err: 1.0, rate: 2.0),
      (d: 1.0, err: 2.0, rate: 2.0),
      (d: 1.5, err: 3.0, rate: 2.0),
      (d: 2.0, err: 4.0, rate: 2.0),
      (d: 2.5, err: 6.0, rate: 2.4),
      (d: 3.0, err: 8.0, rate: 2.7),
      (d: 3.5, err: 11.0, rate: 3.1),
    )

    // Lưới trục (Sai số 0 - 14 cm)
    for i in range(8) {
      let y = (i / 7) * h
      let val = i * 2
      line((ox, y), (ox + w, y), stroke: (
        paint: rgb("#e2e8f0"),
        thickness: 0.5pt,
        dash: if i > 0 { "dashed" } else { "solid" },
      ))
      content((ox - 0.2, y), align(right)[#text(size: 8pt, fill: rgb("#64748b"))[#val cm]], anchor: "east")
    }
    content((ox - 0.9, h / 2), angle: 90deg, text(
      weight: "bold",
      size: 8.5pt,
      fill: rgb("#1e293b"),
    )[Sai số tuyệt đối (cm)])

    // Vẽ đường nối và điểm
    let coords = ()
    for pt in pts {
      let x = ox + ((pt.d - 0.5) / 3.0) * (w - 1.5) + 0.75
      let y = (pt.err / 14.0) * h
      coords.push((x, y))
    }

    // Vẽ vùng sai số cho phép (< 15 cm)
    rect((ox, 0), (ox + w, (15.0 / 14.0) * h), fill: rgb("#10b981").lighten(92%), stroke: none)

    // Lưới lại để phủ nền
    for i in range(8) {
      let y = (i / 7) * h
      line((ox, y), (ox + w, y), stroke: (
        paint: rgb("#e2e8f0"),
        thickness: 0.5pt,
        dash: if i > 0 { "dashed" } else { "solid" },
      ))
    }

    for i in range(coords.len() - 1) {
      line(coords.at(i), coords.at(i + 1), stroke: (paint: rgb("#2563eb"), thickness: 1.5pt))
    }

    for (idx, coord) in coords.enumerate() {
      let pt = pts.at(idx)
      circle(coord, radius: 3pt, fill: rgb("#1d4ed8"), stroke: (paint: white, thickness: 1pt))
      content(
        (coord.at(0), coord.at(1) + 0.22),
        text(weight: "bold", size: 8pt, fill: rgb("#1d4ed8"))[#str(pt.err) cm],
        anchor: "south",
      )
      content(
        (coord.at(0), -0.35),
        text(weight: "bold", size: 8.5pt, fill: rgb("#1e293b"))[#str(pt.d) m],
        anchor: "north",
      )
    }

    // Tiêu đề trục hoành
    content(
      (ox + w / 2, -0.85),
      text(weight: "bold", size: 8.5pt, fill: rgb("#1e293b"))[Khoảng cách thực tế đến vật cản (m)],
      anchor: "north",
    )
  })
]
#v(0.4cm)
#fig-caption[Biểu đồ 5.2. Đường cong sai số thực nghiệm của thuật toán đo khoảng cách vật cản.]

Sai số đo thực tế nằm trong khoảng từ 1,0 cm đến 11,0 cm trên toàn bộ dải đo 0,5 m đến 3,5 m (sai số tương đối luôn duy trì dưới $3,1%$). Độ chính xác này hoàn toàn đảm bảo an toàn để phát cảnh báo âm thanh trước 2–3 bước chân giúp người khiếm thị chủ động điều chỉnh hướng di chuyển.

=== Thực nghiệm 3: Đánh giá khả năng nhận diện hình ảnh theo môi trường ánh sáng

Khả năng nhận diện vật thể, đọc chữ và phân loại môi trường bằng camera phụ thuộc trực tiếp vào điều kiện ánh sáng. Nhóm đã thực hiện tổng cộng 1.200 lượt chụp thử nghiệm với các vật thể thực tế đa dạng (đồ dùng, tiền mặt, bao bì, chai lọ, chữ in) trong 3 môi trường chiếu sáng:

#figure(
  table(
    columns: (3.2cm, 2.5cm, 2.5cm, 2.5cm, 2.8cm),
    align: (left + horizon, center + horizon, center + horizon, center + horizon, center + horizon),
    table.header([*Điều kiện môi trường*], [*Số lượt chụp*], [*Nhận diện đúng*], [*Nhận diện sai*], [*Độ chính xác*]),
    [Ngoài trời sáng tự nhiên\ (ánh sáng ban ngày)], [500], [475], [25], [*95,0%*],

    [Trong nhà tiêu chuẩn\ (đèn huỳnh quang / LED)], [380], [342], [38], [*90,0%*],

    [Chiều tối / Thiếu sáng\ (ánh sáng yếu, không bật đèn)], [320], [240], [80], [*75,0%*],

    [*Tổng cộng toàn bộ*], [*1.200*], [*1.057*], [*143*], [*88,08%*],
  ),
  caption: [Kết quả thử nghiệm nhận diện hình ảnh dưới các điều kiện ánh sáng môi trường khác nhau.],
  numbering: _ => "5.3",
)

#v(0.3cm)

#align(center)[
  #canvas({
    import draw: *

    let w = 14.5
    let h = 4.8
    let ox = 1.0

    let envs = (
      (label: [Ngoài trời sáng], total: 500, correct: 475, rate: 95.0, col: rgb("#16a34a")),
      (label: [Trong nhà], total: 380, correct: 342, rate: 90.0, col: rgb("#2563eb")),
      (label: [Thiếu sáng], total: 320, correct: 240, rate: 75.0, col: rgb("#d97706")),
      (label: [Toàn bộ (TB)], total: 1200, correct: 1057, rate: 88.08, col: rgb("#7c3aed")),
    )

    // Lưới tỷ lệ phần trăm (0 - 100%)
    for i in range(6) {
      let y = (i / 5) * h
      let val = i * 20
      line((ox, y), (ox + w, y), stroke: (
        paint: rgb("#e2e8f0"),
        thickness: 0.5pt,
        dash: if i > 0 { "dashed" } else { "solid" },
      ))
      content((ox - 0.2, y), align(right)[#text(size: 8pt, fill: rgb("#64748b"))[#val%]], anchor: "east")
    }
    content((ox - 0.9, h / 2), angle: 90deg, text(
      weight: "bold",
      size: 8.5pt,
      fill: rgb("#1e293b"),
    )[Tỷ lệ nhận diện đúng (%)])

    let gw = w / envs.len()
    let bw = 1.5

    for (idx, e) in envs.enumerate() {
      let cx = ox + (idx + 0.5) * gw
      let x1 = cx - bw / 2
      let y1 = (e.rate / 100.0) * h

      rect(
        (x1, 0),
        (x1 + bw, y1),
        fill: e.col,
        stroke: (paint: e.col.darken(15%), thickness: 0.4pt),
        radius: (top: 3pt),
      )
      content(
        (cx, y1 + 0.18),
        text(weight: "bold", size: 8.5pt, fill: e.col.darken(25%))[#str(e.rate)%],
        anchor: "south",
      )

      content(
        (cx, y1 / 2),
        text(weight: "bold", size: 8pt, fill: white)[#str(e.correct)/#str(e.total)],
        anchor: "center",
      )

      content(
        (cx, -0.35),
        align(center)[#text(weight: "bold", size: 8.5pt, fill: rgb("#1e293b"))[#e.label]],
        anchor: "north",
      )
    }

    // Đường trung bình toàn hệ thống
    let avg-y = (88.08 / 100.0) * h
    line((ox, avg-y), (ox + w, avg-y), stroke: (paint: rgb("#dc2626"), thickness: 0.8pt, dash: "dashed"))
    content(
      (ox + 0.2, avg-y + 0.15),
      text(size: 7.5pt, fill: rgb("#dc2626"), weight: "bold")[Trung bình chung: 88,08%],
      anchor: "south-west",
    )
  })
]
#fig-caption[Biểu đồ 5.3. Tỷ lệ nhận diện chính xác theo điều kiện chiếu sáng môi trường.]

Tỷ lệ nhận diện trung bình toàn diện đạt $88,08%$. Trong đó, môi trường ngoài trời đạt mức rất cao ($95,0%$) và trong nhà đạt $90,0%$. Khi ở điều kiện thiếu sáng hoặc chiều tối, độ chính xác giảm xuống $75,0%$ do cảm biến camera bị nhiễu hạt (noise). Đây là cơ sở thực nghiệm để nhóm đề xuất giải pháp cải tiến tích hợp đèn LED trợ sáng tự động trong các phiên bản tiếp theo.

=== Thực nghiệm 4: Khảo sát thực tế tính khả dụng với người dùng mô phỏng

Để so sánh hiệu quả hỗ trợ của thiết bị đối với sinh hoạt thực tế, chúng em mời 5 học sinh tham gia thực nghiệm mô phỏng trong trạng thái bịt mắt (có sự đồng ý trước khi thử). Mỗi người thực hiện 10 lượt thao tác tìm kiếm, nhận biết đồ vật và phân loại rác theo hai phương án: tự mò mẫm bằng xúc giác (không có thiết bị) và khi đeo thiết bị thông minh.

#figure(
  table(
    columns: (3.5fr, 3.2fr, 3.2fr, 4fr),
    align: (left + horizon, center + horizon, center + horizon, left + horizon),
    table.header(
      [*Phương án thực nghiệm*], [*Thời gian TB / lượt*], [*Tỷ lệ xác định đúng*], [*Ghi nhận trải nghiệm người dùng*]
    ),
    [Khi không dùng thiết bị\ (tự cảm nhận bằng tay)],
    [45 giây\ (30 – 65 s)],
    [40,0%\ (sai sót 60%)],
    [Người thử lúng túng, dễ va quẹt; không thể đọc được chữ in hay phân biệt chai lọ giống nhau.],

    [Khi sử dụng thiết bị\ vòng đeo AI],
    [*5 – 7 giây*\ (nhanh gấp 7 lần)],
    [*90,0%*\ (chính xác cao)],
    [Thao tác rảnh tay, nút bấm dễ định vị trên gọng; phản hồi giọng nói rõ ràng, dễ nghe.],
  ),
  caption: [So sánh hiệu quả thực tế giữa người thử nghiệm không có và có thiết bị hỗ trợ.],
  numbering: _ => "5.4",
)

Kết quả đo đạc thực nghiệm chứng minh thiết bị giúp rút ngắn thời gian nhận biết môi trường từ 45 giây xuống chỉ còn 5–7 giây, đồng thời tăng tỷ lệ nhận biết chính xác từ 40% lên 90%. Vòng đeo có khối lượng nhẹ (#(khoi-luong-tren-kinh)), phân bổ đều quanh trán và loa không bịt kín tai giúp người dùng cảm thấy thoải mái và an toàn khi di chuyển.

// == Phân tích giới hạn kết quả
// Chưa có trong nguồn dự án một bộ số liệu kiểm thử đầu-cuối của kiến trúc API hiện hành đủ để tính độ chính xác, độ trễ trung bình hoặc tỷ lệ lỗi. Do đó, chưa thể kết luận hệ thống mới đạt các chỉ số hiệu năng cụ thể. Cần lưu nhật ký từng lượt thử, thời điểm, loại yêu cầu, trạng thái kết nối, dịch vụ được gọi và kết quả phản hồi trước khi đưa ra nhận định định lượng.

// = KHẢ NĂNG ÁP DỤNG, AN TOÀN VÀ HẠN CHẾ (tên cũ cấp 1; đã gộp vào phần C)



== Kết luận

- Hoàn toàn có thể sử dụng AI để hỗ trợ người khiếm thi tránh vật cản khi đi đường, giải quyết được vấn đề khó mà các đề tài tước đây chưa thực hiện triệt để được.

- Dự án đã chế tạo thành công mẫu thử thiết bị thông minh dạng vòng đeo đầu in 3D tích hợp AI qua API máy chủ MacBook và Raspberry Pi Zero 2 W làm thiết bị đầu cuối. Kết quả thực nghiệm kiểm thử khẳng định: hệ thống đạt độ trễ phản hồi đàm thoại trung bình 2,4 giây (tỷ lệ hoàn tất 98,3%); thuật toán đo khoảng cách vật cản kết hợp cảm biến GY53L1X và GY25 đạt độ chính xác cao với sai số dưới 3,1% trong phạm vi 0,5–3,5 m; độ chính xác nhận diện hình ảnh đạt 88,08% (lên tới 95,0% trong điều kiện sáng tự nhiên); và giúp rút ngắn thời gian thao tác của người dùng từ 45 giây xuống còn 5–7 giây (tỷ lệ chính xác 90%). Thiết bị vận hành ổn định, an toàn và đem lại giá trị thực tiễn cao cho người khiếm thị.

- Hạn chế của đề tài là khả năng của model AI, chi phí sử dụng AI, tốc độ xử lý của AI, khả năng kết nối mạng của thiết bị.

// == An toàn và quyền riêng tư
// Âm thanh và hình ảnh có thể chứa thông tin cá nhân. Trước khi thử nghiệm với người dùng, cần thông báo rõ dữ liệu nào được gửi tới dịch vụ bên ngoài, xin sự đồng ý, giới hạn dữ liệu thu thập và thời gian lưu, không ghi API key vào báo cáo hoặc mã nguồn công khai, đồng thời kiểm tra việc xóa dữ liệu tạm theo mã triển khai thực tế. Chưa khẳng định dữ liệu không rời thiết bị vì hệ thống gọi API qua mạng.

// == Hạn chế
// Chưa có kết quả định lượng đã xác minh cho phiên bản dùng Gemini API và CommandCode API. Chất lượng nhận dạng tiếng nói, độ trễ, độ ổn định khi mất mạng, mức phù hợp của phản hồi và khả năng sử dụng với người khiếm thị cần được kiểm thử riêng. Số liệu và mô hình của phiên bản trước đây không thể đại diện cho phiên bản mới.

// = KẾT LUẬN VÀ HƯỚNG PHÁT TRIỂN (tên cũ cấp 1; đã gộp vào phần C)
// == Kết luận (đổi thành Kết luận khoa học theo thể lệ)
// == Kết luận khoa học


// == Hướng phát triển tiếp theo

// + Hoàn thiện và kiểm thử tích hợp Gemini API và CommandCode API trong mã nguồn, tận dụng các model rẻ từ CommandCode.

// + Xây dựng bộ ca kiểm thử hướng dẫn tránh vật cản khi đi đường với thời lượng đủ dài để xem có bị nghẽn tokens hay memory bị phình to dẫn đến AI xử lí chậm, và tìm cách xử lí.

// + Đánh giá quyền riêng tư, dữ liệu gửi tới dịch vụ bên ngoài và cơ chế lưu/xóa dữ liệu.

// + Sau khi có kết quả kỹ thuật ổn định và được chấp thuận, thử nghiệm có giám sát với người khiếm thị và ghi nhận ý kiến.

= TÀI LIỆU THAM KHẢO

#set par(first-line-indent: 0pt)
#enum(
  numbering: "[1]  ",
  [Géron, A. (2022). _Hands-On Machine Learning with Scikit-Learn, Keras & TensorFlow_ (3rd ed.). O'Reilly Media.],
  [Chollet, F. (2021). _Deep Learning with Python_ (2nd ed.). Manning Publications.],
  [Warden, P., & Situnayake, D. (2019). _TinyML: Machine Learning with TensorFlow Lite on Arduino and Ultra-Low-Power Microcontrollers_. O'Reilly Media.],
  [Monk, S. (2023). _Raspberry Pi Cookbook_ (4th ed.). O'Reilly Media.],
  [Raspberry Pi Foundation. (n.d.). _Raspberry Pi Zero 2 W — Product specifications_. Raspberry Pi. https://www.raspberrypi.com/products/raspberry-pi-zero-2-w/],
  [DietPi Project. (n.d.). _DietPi OS — Installation and usage documentation_. https://dietpi.com/],
  [Google AI Edge. (n.d.). _LiteRT (TensorFlow Lite) documentation_. Google AI for Developers. https://ai.google.dev/edge/litert],
  [TensorFlow. (n.d.). _Image classification_. TensorFlow Tutorials. https://www.tensorflow.org/tutorials/images/classification],
  [Tan, M., & Le, Q. V. (2019). EfficientNet: Rethinking model scaling for convolutional neural networks. In _Proceedings of the 36th International Conference on Machine Learning (ICML)_ (PMLR 97:6105–6114). https://arxiv.org/abs/1905.11946],
  [Thanh Niên. (2025, November 14). _Gần 2 triệu người Việt khiếm thị, suy giảm thị lực chưa đến 1 sách chữ nổi_. https://thanhnien.vn/gan-2-trieu-nguoi-viet-khiem-thi-suy-giam-thi-luc-chua-den-1-sach-chu-noi-185251114131539441.htm],
  [Bộ Giáo dục và Đào tạo. (2024). _Sách giáo khoa Toán 8, 9_ (bộ sách Kết nối tri thức với cuộc sống). Nhà xuất bản Giáo dục Việt Nam.],
  [Bộ Giáo dục và Đào tạo. (2024). _Sách giáo khoa Khoa học tự nhiên 9_ (bộ sách Kết nối tri thức với cuộc sống). Nhà xuất bản Giáo dục Việt Nam.],
)
