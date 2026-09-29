# NHẬT KÍ THỰC HIỆN DỰ ÁN

> **Dự án:** Hệ thống hỗ trợ phát hiện té ngã và cảnh báo cho người thân  
> **Ghi chú:** Nhật kí được cập nhật theo tiến trình thực tế của dự án. Các đường link chỉ được ghi khi đã có đường link trao đổi xác định; không tự tạo hoặc suy đoán liên kết.

## 1. Hình thành ý tưởng và lựa chọn thiết bị

1. **Trao đổi ban đầu về ý tưởng dự án.**  
   Trao đổi với ChatGPT về ý tưởng xây dựng hệ thống hỗ trợ phát hiện té ngã và cảnh báo cho người thân. Phân tích các phương án triển khai, thống nhất lựa chọn các module, cảm biến và thiết bị cần thiết để tiến hành dự án.  
   [Nội dung trao đổi](https://chatgpt.com/share/6aab8abc-eed0-83ec-b2c5-4269d9e4b5f7)

2. **Thống nhất các nội dung chính của dự án.**  
   Tiếp tục trao đổi để xác định mục tiêu, đối tượng sử dụng, các chức năng chính của hệ thống, phương án phát hiện té ngã, cách cảnh báo và phương thức liên lạc với người thân.  
   [Nội dung trao đổi](https://chatgpt.com/share/6aab8abc-eed0-83ec-b2c5-4269d9e4b5f7)

3. **Tìm mua thiết bị và nghiên cứu các phương án thay thế.**  
   Tìm kiếm các module và cảm biến phù hợp để chế tạo thiết bị. Trong quá trình này, tiếp tục trao đổi với ChatGPT để so sánh thông số kỹ thuật và lựa chọn các thiết bị thay thế trong trường hợp linh kiện dự kiến ban đầu chưa phù hợp, khó mua hoặc hết hàng.  
   [Nội dung trao đổi](https://chatgpt.com/share/6aab8abc-eed0-83ec-b2c5-4269d9e4b5f7)

## 2. Tách dự án thành Android và ESP32

4. **Tách dự án thành hai hướng phát triển: Android và ESP32.**  
   Do hệ thống dự kiến sử dụng đồng thời điện thoại Android và thiết bị phần cứng ESP32, tiến hành xây dựng kế hoạch riêng cho từng thành phần, đồng thời thống nhất cách hai hệ thống phối hợp và trao đổi dữ liệu với nhau.

   1. **Android:** xây dựng kế hoạch cho ứng dụng trên điện thoại, bao gồm sử dụng cảm biến có sẵn của điện thoại, phát hiện sự kiện té ngã, cảnh báo, quản lí người thân, lấy vị trí và giao tiếp với thiết bị ESP32.  
      [Nội dung trao đổi](https://chatgpt.com/share/6aab8abc-eed0-83ec-b2c5-4269d9e4b5f7)

   2. **ESP32:** xây dựng kế hoạch cho thiết bị đeo sử dụng ESP32 và các cảm biến ngoài, thu thập dữ liệu chuyển động, độ cao/áp suất và truyền dữ liệu sang ứng dụng Android.  
      [Nội dung trao đổi](https://chatgpt.com/share/6aab8abc-eed0-83ec-b2c5-4269d9e4b5f7)

## 3. Tìm hiểu công cụ AI và môi trường phát triển

5. **Tìm hiểu và sử dụng các công cụ AI hỗ trợ phát triển dự án.**

   1. **Tìm hiểu Hermes Agent:** nghiên cứu cách sử dụng Hermes làm công cụ quản lí và điều phối công việc lập trình, giao nhiệm vụ cho các AI khác và theo dõi tiến trình thực hiện.

   2. **Tìm hiểu Codex:** nghiên cứu khả năng sử dụng Codex để phân tích mã nguồn, xây dựng backend, xử lí logic chương trình và kiểm thử.

   3. **Tìm hiểu Antigravity (AGY):** nghiên cứu khả năng sử dụng AGY và các model AI có sẵn để hỗ trợ xây dựng giao diện và các thành phần frontend của ứng dụng.

   4. **Tìm hiểu môi trường Linux và GitHub:** thiết lập môi trường phát triển trên Linux, sử dụng terminal, SSH, Git và GitHub để quản lí mã nguồn, lưu trữ dự án và hỗ trợ các AI agent làm việc trên cùng repository.  
      [Nội dung trao đổi](https://chatgpt.com/share/6aab8e1b-2850-83ec-ae5f-7b73ea95ada3)

   5. **Tìm hiểu biện pháp giảm lượng token Hermes sử dụng:** xây dựng quy trình khảo sát mã nguồn có trọng tâm, ưu tiên sử dụng các lệnh Linux như `rg`, `grep`, `sed` để tìm đúng file và chỉ đọc các đoạn mã cần thiết thay vì đọc toàn bộ repository. Mục đích là giảm lượng token tiêu tốn và tăng hiệu quả làm việc của AI.  
      [Nội dung trao đổi](https://chatgpt.com/share/6aab8abc-eed0-83ec-b2c5-4269d9e4b5f7)

## 4. Xây dựng phiên bản Android thử nghiệm

6. **Xây dựng ứng dụng Android với sự hỗ trợ của ChatGPT và Hermes.**

   1. **Thống nhất chức năng và giao diện ứng dụng Android.**  
      Trao đổi với ChatGPT để xác định các chức năng chính của phần mềm, cách sử dụng cảm biến có sẵn trên điện thoại, cơ chế phát hiện té ngã, cảnh báo SOS và các yêu cầu về giao diện phù hợp với người cao tuổi.  
      [Nội dung trao đổi](https://chatgpt.com/share/6aab8abc-eed0-83ec-b2c5-4269d9e4b5f7)

   2. **Sử dụng ChatGPT tạo prompt giao nhiệm vụ cho Hermes.**  
      Chuyển các yêu cầu đã thống nhất thành prompt chi tiết để Hermes phân công các AI agent xây dựng frontend, backend, logic xử lí và kiểm thử ứng dụng Android.  
      [Nội dung trao đổi](https://chatgpt.com/share/6aab8e58-152c-83ec-91fb-2cf63e07d468)

   3. **Tạo APK thử nghiệm và kiểm thử phần mềm.**  
      Biên dịch ứng dụng thành file APK để cài đặt trên điện thoại Android thực tế. Kiểm tra khả năng hoạt động của giao diện và khả năng đọc dữ liệu từ các cảm biến có sẵn trên điện thoại. Sau quá trình thử nghiệm, tiếp tục trao đổi với ChatGPT để phát hiện hạn chế và tạo prompt cho Hermes chỉnh sửa chương trình.  
      [Tạo và kiểm thử APK](https://chatgpt.com/share/6aab9036-7c60-83ec-988d-f0d6da743012)  
      [Trao đổi để tiếp tục chỉnh sửa](https://chatgpt.com/share/6aab8eb7-ad18-83ec-99cd-afbbb23e0c15)

   4. **Tiếp tục hoàn thiện phiên bản thử nghiệm.**  
      Phiên bản thử nghiệm ban đầu chủ yếu kiểm tra việc kết nối các cảm biến có sẵn trên Android và xây dựng giao diện; chưa hoàn thiện các chức năng liên quan đến SIM, SMS và cuộc gọi. Vì vậy tiếp tục thực hiện các công việc sau:

      1. **Đưa dự án lên GitHub để lưu trữ và quản lí phiên bản.**  
         Thiết lập Git, đưa mã nguồn Android cùng các thành phần liên quan của dự án lên GitHub để tránh mất dữ liệu và thuận tiện theo dõi quá trình chỉnh sửa.  
         [Nội dung trao đổi](https://chatgpt.com/share/6aab9036-7c60-83ec-988d-f0d6da743012)

      2. **Hoàn thiện kiến trúc frontend – backend và giao tiếp API.**  
         Phân tách rõ phần giao diện, logic xử lí và backend; thống nhất các API dùng để truyền nhận dữ liệu giữa ứng dụng Android, backend và thiết bị ESP32. Việc này giúp các thành phần có thể phát triển độc lập nhưng vẫn sử dụng chung một cấu trúc dữ liệu.

      3. **Đưa chức năng thiết lập ngưỡng phát hiện té ngã vào Android.**  
         Do dự án vẫn đang trong giai đoạn thử nghiệm, chưa thể xác định ngay một bộ ngưỡng té ngã chính xác. Vì vậy bổ sung giao diện cho phép thay đổi các ngưỡng phát hiện té ngã trực tiếp trên ứng dụng Android. Đồng thời thiết kế chức năng **Save/Save As**, cho phép lưu nhiều bộ thông số thử nghiệm với các tên 1, 2, 3,... để so sánh và lựa chọn bộ giá trị phù hợp nhất sau khi thực nghiệm.  
         [Nội dung trao đổi](https://chatgpt.com/share/6aab8fdb-8d18-83ec-ac4e-89bd694e9d5f)

      4. **Thiết kế lại giao diện phù hợp với người sử dụng.**  
         Phân tích giao diện hiện tại và nhận thấy người cao tuổi không cần theo dõi quá nhiều thông số kỹ thuật. Vì vậy thiết kế lại màn hình chính theo hướng đơn giản, dễ quan sát, các nút lớn và dễ thao tác. Nút SOS/Hủy cảnh báo được đặt ở vị trí trung tâm, các chức năng quan trọng khác được bố trí xung quanh. Đồng thời điều chỉnh bố cục để tận dụng toàn bộ màn hình và cân đối hơn.  
         [Nội dung trao đổi](https://chatgpt.com/share/6aab8abc-eed0-83ec-b2c5-4269d9e4b5f7)

## 5. Hoàn thiện quy trình phát triển và quản lí dự án

7. **Thiết lập Hermes làm bộ phận điều phối dự án.**  
   Trong quá trình phát triển, thống nhất không để một AI duy nhất tự thực hiện toàn bộ công việc. Hermes được giao vai trò quản trị dự án: khảo sát có trọng tâm, chốt yêu cầu và interface trước khi lập trình, chia nhiệm vụ cho các worker, kiểm tra kết quả và tích hợp mã nguồn.

   - Hermes chịu trách nhiệm điều phối chung.
   - AGY ưu tiên xử lí phần frontend và giao diện Android.
   - Codex/Sol ưu tiên xử lí backend, logic, API, cảm biến, kiểm thử và các phần cần suy luận kỹ thuật.
   - Không giao hai worker chỉnh sửa cùng một file đồng thời.
   - Chỉ đọc các đoạn mã cần thiết bằng `rg`, `grep`, `sed`; không đọc lại toàn bộ repository nếu thông tin đã được ghi trong tài liệu kiến trúc.
   - Yêu cầu worker chạy test trước khi báo hoàn thành.

8. **Thiết lập quy trình làm việc từ xa với máy Linux.**  
   Máy Linux được sử dụng làm máy phát triển chính và có thể truy cập từ xa qua SSH/Tailscale. Nhờ đó có thể giao việc cho Hermes từ điện thoại hoặc MacBook, sau đó theo dõi tiến trình trên cùng repository mà không phụ thuộc vào một thiết bị duy nhất.

9. **Chuẩn hóa việc lưu mã nguồn và xử lí xung đột Git.**  
   Tiếp tục đưa các thành phần Android, ESP32, protocol thử nghiệm và script cấu hình lên GitHub. Trong quá trình đó đã xử lí các trạng thái Git như file mới, file đang staged, unmerged paths và hoàn tất việc đồng bộ repository.

## 6. Xây dựng backend và kết nối Android – backend

10. **Thiết kế lại backend theo hướng rõ ràng và tối giản.**  
    Sau khi phiên bản Android ban đầu hoạt động, tiến hành xây dựng backend chính thức thay cho các đoạn logic thử nghiệm rời rạc. Kiến trúc hiện tại được khảo sát và ghi lại trước khi thay đổi mã nguồn.

    Các nguyên tắc được thống nhất:
    - Backend sử dụng REST API với namespace `/api/v1`.
    - Android và ESP32 sử dụng chung các model/state cần thiết.
    - Response của API có cấu trúc thống nhất.
    - Hỗ trợ SOS, countdown, ACK, quản lí contacts và tiếp nhận dữ liệu cảm biến.
    - Không viết lại toàn bộ dự án; giữ lại các phần đang hoạt động tốt.
    - Chốt contract trước rồi mới cho các worker triển khai song song.

11. **Kết nối frontend và backend qua API.**  
    Tạo prompt cho Hermes để rà soát frontend Android và backend hiện có, xác định các API còn thiếu và kết nối hai thành phần. Mục tiêu là giao diện không chứa logic backend không cần thiết và backend cung cấp dữ liệu qua interface rõ ràng.

12. **Kiểm tra lại kiến trúc trước khi triển khai backend.**  
    Hermes thực hiện giai đoạn discovery, ghi bản đồ kiến trúc vào `.ai/architecture.md` và trạng thái công việc vào `.ai/task_on_progress.md`. Sau đó mới chuyển sang giai đoạn contract và implementation. Cách làm này giúp tránh việc AI sửa mã ngay khi chưa hiểu cấu trúc dự án.

## 7. Hoàn thiện hệ thống ngưỡng phát hiện té ngã

13. **Đưa các ngưỡng phát hiện té ngã ra khỏi mã nguồn cố định.**  
    Qua thử nghiệm nhận thấy ứng dụng đã đọc được cảm biến điện thoại nhưng các giá trị mặc định chưa đủ cơ sở để khẳng định là ngưỡng té ngã chính xác. Vì vậy quyết định đưa toàn bộ ngưỡng cần thử nghiệm lên phần mềm Android để người nghiên cứu có thể tinh chỉnh.

14. **Thiết kế hệ thống lưu nhiều bộ thông số thử nghiệm.**  
    Bổ sung yêu cầu:
    - Có nút **Save** để cập nhật bộ thông số đang sử dụng.
    - Có nút **Save As** để tạo một bộ thông số mới.
    - Cho phép lưu nhiều bộ giá trị khác nhau.
    - Các bộ thử nghiệm ban đầu được đặt tên theo số thứ tự: `1`, `2`, `3`, ...
    - Có thể chọn một bộ thông số đã lưu để sử dụng lại.
    - Sau khi thu thập đủ dữ liệu thực nghiệm sẽ so sánh và chọn bộ ngưỡng phù hợp nhất.

15. **Phân công AI theo chuyên môn khi xây dựng chức năng ngưỡng.**  
    AGY được ưu tiên thực hiện giao diện cấu hình và quản lí các bộ ngưỡng; Codex/Sol xử lí logic lưu dữ liệu, kiểm tra hợp lệ, backend và API. Interface được chốt trước để tránh xung đột khi hai worker làm song song.

## 8. Thiết kế giao tiếp Android – ESP32

16. **Thống nhất Android không chỉ là thiết bị nhận cảnh báo mà còn là thiết bị cấu hình cho ESP32.**  
    Trong giai đoạn thử nghiệm, các ngưỡng phát hiện té ngã của ESP32 có thể phải thay đổi nhiều lần. Vì vậy quyết định cho phép Android gửi các thông số cấu hình xuống ESP32 thay vì phải nạp lại firmware sau mỗi lần chỉnh.

17. **Thiết kế frontend và backend cho việc setup ESP32.**  
    Yêu cầu Hermes phân công:
    - AGY thiết kế phần giao diện cấu hình ESP32 trên Android.
    - Sol xử lí backend, model dữ liệu, API và logic truyền nhận.
    - Các thông số phải có thể đọc từ ESP32, hiển thị trên Android, chỉnh sửa và gửi ngược trở lại ESP32.
    - Android và ESP32 phải dùng cùng tên biến/contract để tránh sai khác dữ liệu.

18. **Bổ sung dữ liệu áp suất/độ cao vào hệ thống.**  
    Thiết bị ESP32 dự kiến sử dụng BMP390 để đo biến thiên áp suất, hỗ trợ đánh giá sự thay đổi độ cao khi xảy ra té ngã. Đồng thời thống nhất rằng nếu điện thoại Android có cảm biến áp suất thì cũng khai thác dữ liệu này để tăng lượng thông tin phục vụ thử nghiệm.

## 9. Hoàn thiện giao diện Android cho người cao tuổi

19. **Đánh giá lại giao diện từ góc nhìn người sử dụng thực tế.**  
    Sau khi cài bản APK thử nghiệm, nhận thấy giao diện còn thiên về hiển thị thông số kỹ thuật và chưa tối ưu cho người cao tuổi. Thống nhất rằng màn hình chính phải ưu tiên hành động quan trọng hơn là trình bày nhiều số liệu.

20. **Thiết kế lại màn hình chính.**  
    Chọn bố cục:
    - Nút SOS/Hủy SOS lớn ở trung tâm.
    - Bốn khu vực chức năng bố trí xung quanh.
    - Các block được vẽ lại để liên kết trực quan với vòng tròn SOS.
    - Giao diện sử dụng toàn màn hình, tránh lệch nhiều về phía trên.
    - Chữ và nút phải đủ lớn, dễ đọc, dễ chạm.
    - Các thông số nghiên cứu chi tiết không đưa lên màn hình chính của người dùng phổ thông.

21. **Chuyển quản lí số điện thoại người thân vào phần Cài đặt.**  
    Thống nhất số điện thoại liên hệ khẩn cấp không nên cố định trong mã nguồn. Người dùng có thể:
    - Thêm người thân.
    - Sửa thông tin.
    - Xóa người thân.
    - Chọn người/những người nhận cảnh báo.
    Việc quản lí này được đưa vào mục **Cài đặt** để màn hình chính vẫn đơn giản.

## 10. Hoàn thiện chức năng vị trí và SOS

22. **Phát hiện vấn đề ở chức năng “Vị trí”.**  
    Trong phiên bản đang thử nghiệm, nút “Vị trí” chưa đáp ứng đúng mục tiêu cứu hộ: khi thao tác có thể hướng tới việc mở bản đồ trên điện thoại người bị nạn, trong khi yêu cầu thực tế là **người thân phải nhận được tọa độ/vị trí của người bị ngã**.

23. **Thiết kế lại luồng gửi vị trí.**  
    Thống nhất luồng mới:
    1. Ứng dụng lấy vị trí hiện tại của người sử dụng.
    2. Tạo đường link vị trí có thể mở trên bản đồ.
    3. Gửi đường link này qua tin nhắn cho người thân đã cấu hình.
    4. Người thân có thể bấm vào link để xem vị trí của người bị nạn.
    5. Không yêu cầu người bị ngã phải tự mở Google Maps.

24. **Thiết kế lại cuộc gọi SOS.**  
    Sau khi gửi tin nhắn vị trí, ứng dụng tiến hành gọi người thân. Nội dung cảnh báo dự kiến được phát lặp lại:
    > “Tôi đã bị ngã, vị trí tại đường link đã gửi qua tin nhắn.”

    Mục tiêu là ngay cả khi người bị ngã mất ý thức hoặc không thể nói, người thân vẫn nhận được thông tin rằng đã xảy ra sự cố và biết nơi cần đến.

25. **Xử lí trường hợp người bị ngã mất ý thức.**  
    Trước đây có phương án yêu cầu người dùng xác nhận sau cảnh báo. Qua phân tích nhận thấy cách này không phù hợp nếu người bị ngã bị choáng, đột quỵ hoặc mất ý thức. Vì vậy logic được chuyển theo hướng:
    - Khi hệ thống xác định có nguy cơ té ngã, bắt đầu countdown ngắn.
    - Nếu người dùng tỉnh táo và phát hiện cảnh báo sai, họ có thể bấm hủy.
    - Nếu không có thao tác hủy, hệ thống tự động chuyển sang quy trình SOS.
    - Việc gửi vị trí và cảnh báo không phụ thuộc vào khả năng thao tác tiếp của nạn nhân.

26. **Tích hợp chức năng vị trí, SMS và gọi SOS với backend hiện có.**  
    Giao Hermes khảo sát code hiện tại trước khi sửa, chốt contract giữa Android và backend, sau đó phân công:
    - AGY xử lí giao diện liên quan.
    - Codex/Sol xử lí vị trí, SMS, logic cuộc gọi, backend và kiểm thử.
    - Không cho hai worker cùng sửa một file.
    - Giữ nguyên các chức năng đang chạy tốt và chỉ bổ sung phần cần thiết.

## 11. Trạng thái hiện tại của dự án — 17/09/2026

27. **Android**
    - Đã có ứng dụng thử nghiệm và APK có thể cài trên thiết bị thực.
    - Đã đọc được một số cảm biến có sẵn trên điện thoại.
    - Đã có màn hình chính và đang tiếp tục tối ưu cho người cao tuổi.
    - Đã xác định yêu cầu quản lí contacts.
    - Đã thiết kế cơ chế cấu hình nhiều bộ ngưỡng phát hiện té ngã.
    - Đang hoàn thiện luồng SOS, vị trí, SMS và cuộc gọi.
    - Đang kết nối frontend với backend qua API.

28. **Backend**
    - Đã thực hiện discovery kiến trúc.
    - Đã xác định contract REST `/api/v1`.
    - Đã thống nhất các nhóm chức năng: SOS, countdown, ACK, contacts CRUD và sensor ingestion.
    - Tiếp tục triển khai theo hướng contract-first và kiểm thử trước khi tích hợp.

29. **ESP32**
    - Đã có kế hoạch riêng cho firmware và cảm biến.
    - Dự kiến sử dụng dữ liệu chuyển động kết hợp BMP390.
    - Android sẽ có khả năng cấu hình ngưỡng cho ESP32 trong quá trình thử nghiệm.
    - Cần tiếp tục hoàn thiện protocol truyền nhận, firmware thực tế và thử nghiệm với phần cứng.

30. **Quản lí dự án**
    - Mã nguồn được lưu trên GitHub.
    - Máy Linux được sử dụng làm môi trường phát triển và điều phối AI agent.
    - Hermes giữ vai trò quản trị/điều phối; AGY và Codex/Sol đảm nhiệm các nhóm công việc phù hợp.
    - Áp dụng quy trình dùng `rg`, `grep`, `sed` để giảm token và tránh đọc lại toàn bộ repository.
    - Các thay đổi lớn phải được ghi lại trong tài liệu kiến trúc/trạng thái trước khi tiếp tục triển khai.

---

## 12. Công việc tiếp theo

31. Hoàn thiện và kiểm thử thực tế chức năng gửi tọa độ qua SMS.
32. Hoàn thiện quy trình gọi SOS tự động và cách kết thúc cảnh báo an toàn.
33. Kiểm tra quyền Android liên quan đến vị trí, SMS, cuộc gọi và hoạt động nền.
34. Hoàn thiện quản lí danh sách người thân trong phần Cài đặt.
35. Hoàn thiện API cấu hình và đồng bộ ngưỡng Android ↔ backend ↔ ESP32.
36. Lắp ráp ESP32 với các cảm biến thực tế và kiểm tra truyền dữ liệu với Android.
37. Xây dựng quy trình thu thập dữ liệu thực nghiệm để xác định ngưỡng té ngã phù hợp.
38. Thiết kế các tình huống thử nghiệm: đi bộ, ngồi xuống, nằm xuống, trượt, ngã từ tư thế đứng, ngã từ ghế, bám vào vật khi mất thăng bằng,...
39. Ghi lại kết quả từng lần thử để so sánh các bộ ngưỡng `1`, `2`, `3`, ...
40. Sau khi có đủ dữ liệu, lựa chọn bộ thông số tốt hơn và đánh giá tỉ lệ phát hiện đúng/sai.

---

## Quy ước cập nhật nhật kí

- Chỉ ghi những công việc đã thực hiện hoặc đã thống nhất rõ trong dự án.
- Không ghi một ý tưởng đang cân nhắc như thể đã hoàn thành.
- Mỗi thay đổi quan trọng nên ghi: **vấn đề → quyết định → cách thực hiện → kết quả**.
- Khi có bản APK, firmware, commit hoặc kết quả thử nghiệm mới, bổ sung ngày và phiên bản nếu xác định được.
- Không tự tạo đường link ChatGPT; chỉ thêm link khi có link chia sẻ thực tế.
- File này là nguồn nhật kí chính của dự án và sẽ tiếp tục được cập nhật ở các lần làm việc sau.


## 13. Kiểm thử các bản APK đã tạo — 17/09/2026

### 13.1. Phạm vi kiểm thử

Các file APK được kiểm tra theo đúng thứ tự phát triển:

1. `app-debug.apk` — bản đầu tiên.
2. `app-debug (1).apk`.
3. `app-debug (2).apk`.
4. `app-debug (3).apk`.
5. Không có `app-debug (4).apk` do tải nhầm/trùng với bản (3).
6. `app-debug (5).apk` — bản mới nhất trong đợt kiểm tra này.

Việc kiểm thử trên server được chia thành hai mức:

- **Kiểm thử tĩnh APK:** kiểm tra tính toàn vẹn file, cấu trúc DEX, manifest, permissions, package, các lớp chương trình, API và các thành phần được đóng gói.
- **Kiểm thử chạy thực tế:** server hiện tại không có sẵn Android SDK, ADB và Android Emulator nên chưa thể khởi động APK như trên điện thoại Android. Vì vậy các nhận xét dưới đây không được coi là thay thế cho kiểm thử trên thiết bị thật.

Tất cả 5 file APK đều vượt qua kiểm tra tính toàn vẹn ZIP/APK, không phát hiện file đóng gói bị hỏng.

### 13.2. Phát hiện về phiên bản

Kiểm tra SHA-256 cho thấy:

- `app-debug.apk`: `a3bba83a08a0d7c77c189df001249a28adc468f85a113bdfbc6d3cb3aaf32ecd`
- `app-debug (1).apk`: `743b3550128674ea5098cbb8aa92c91ae34005a88bf834464f7bb9d99d5fbe5f`
- `app-debug (2).apk`: `b8b30208c393f540d9dbfd7ce32f7a6d921458c036ff2ecabd7e5d1306310b01`
- `app-debug (3).apk`: `b8b30208c393f540d9dbfd7ce32f7a6d921458c036ff2ecabd7e5d1306310b01`
- `app-debug (5).apk`: `785bcc5657895c25fcce1139c20134fcbf2d4102c26798fb1274855ac4d85ace`

**Nhận xét quan trọng:** `app-debug (2).apk` và `app-debug (3).apk` giống hệt nhau ở mức byte, không chỉ giống chức năng. Vì vậy trong quá trình phân tích có thể xem hai file này là cùng một bản build. Bản (4) theo ghi chú của dự án cũng là bản tải nhầm/trùng với bản (3), nên giai đoạn này thực tế không tạo ra một phiên bản phần mềm mới.

### 13.3. Bản đầu tiên — `app-debug.apk`

Bản đầu có package:

`vn.nckh27pa.fallsafe`

và tên ứng dụng được đóng gói là **FallSafe THỬ NGHIỆM**.

Các thành phần chính đã xuất hiện:

- `MainActivity`.
- `MonitoringService`.
- `PhoneSensorCollector`.
- `PhoneNormalizer`.
- `PhoneInputPipeline`.
- `DemoDetector`.
- `DemoController`.
- `Esp32PacketDecoder`.
- `Esp32SensorPacket`.

Như vậy ngay từ bản đầu ứng dụng đã có nền tảng thu nhận cảm biến điện thoại và cấu trúc giải mã gói dữ liệu ESP32. Đây vẫn là kiến trúc thử nghiệm, thể hiện qua nhiều lớp mang tên `Demo...`.

Manifest có khai báo cảm biến gia tốc và các quyền phục vụ foreground service/đánh thức thiết bị. Chưa phát hiện quyền Internet, vị trí, SMS, gọi điện hoặc Bluetooth trong manifest.

**Nhận xét:** bản đầu phù hợp với mục tiêu kiểm tra cảm biến và logic phát hiện ban đầu, chưa phải bản có khả năng cứu hộ hoàn chỉnh.

### 13.4. Bản `app-debug (1).apk`

So với bản đầu, dung lượng APK tăng nhẹ và số lớp ứng dụng tăng đáng kể. Xuất hiện rõ các thành phần giao diện mới như:

- `HomeScreenKt`.
- `MainScreenStatus`.
- nhiều thành phần giao diện Compose liên quan tới nút hành động trung tâm.

Manifest bổ sung quyền `VIBRATE`.

**Nhận xét:** đây là giai đoạn chuyển từ giao diện thử nghiệm ban đầu sang màn hình chính có cấu trúc rõ hơn, phù hợp với quá trình thiết kế lại UI cho người cao tuổi. Tuy nhiên lớp nền về cảm biến và ESP32 vẫn chủ yếu giữ nguyên.

### 13.5. Bản `app-debug (2).apk` và `(3).apk`

Hai file này giống hệt nhau.

So với bản (1), xuất hiện các lớp mới liên quan tới quản lí người thân:

- `EmergencyContact`.
- `ContactRepository`.
- `ContactValidator`.
- `ContactsScreenKt`.
- `InMemoryContactRepository`.
- `SharedPrefsContactRepository`.

Ngoài ra xuất hiện các thành phần tạo hình giao diện như `ConcaveCutoutShape`, `CutoutCorner`, cho thấy giao diện màn hình chính tiếp tục được chỉnh theo bố cục có vùng lõm/bo quanh nút SOS trung tâm.

**Nhận xét:** giai đoạn này đã bắt đầu biến yêu cầu “thêm, sửa, xóa số điện thoại người thân trong Cài đặt” thành cấu trúc chương trình thực tế. Việc sử dụng `SharedPrefsContactRepository` cho thấy danh sách liên hệ đã được chuẩn bị để lưu cục bộ thay vì chỉ tồn tại tạm thời trong bộ nhớ.

### 13.6. Bản mới nhất — `app-debug (5).apk`

Bản (5) có thay đổi lớn nhất trong chuỗi APK đã kiểm tra. Dung lượng tăng lên khoảng 11,7 MB, số DEX tăng từ 6 lên 7 và xuất hiện nhiều nhóm chức năng mới.

#### a. Hệ thống cấu hình ngưỡng té ngã

Xuất hiện các lớp:

- `FallDetectionCalibrationScreenKt`.
- `FallDetectionConfig`.
- `FallDetectionObservation`.
- `FallDetectionProfile`.
- `FallDetectionProfileRepository`.
- `FallDetectionProfileStorage`.
- `GsonFallDetectionProfileRepository`.
- `SharedPreferencesFallDetectionProfileStorage`.
- `InMemoryFallDetectionProfileStorage`.
- `CalibrationDraft`.
- `CalibrationValidation`.
- `DetectionPhase`.

Trong mã đóng gói có dấu hiệu của chức năng **Save As**.

**Nhận xét:** yêu cầu đưa ngưỡng phát hiện té ngã lên Android để tinh chỉnh trong quá trình nghiên cứu đã được triển khai thành một subsystem riêng, không còn chỉ là các hằng số nằm trong detector. Đây là thay đổi phù hợp với mục tiêu lưu nhiều bộ thông số thử nghiệm để sau này so sánh.

#### b. Backend và REST API

Bản (5) xuất hiện đầy đủ nhóm lớp:

- `ApiClient`.
- `ApiConfig`.
- `ApiRepository`.
- `ApiService`.
- `Envelope`.
- `ApiError`.
- `ApiResult`.
- `SensorRequest`.
- `HeartbeatRequest`.
- `EventRequest`.
- `ActionRequest`.
- `ContactDto`.
- `SyncCoordinator`.
- `SyncOutbox`.
- `SyncOperation`.
- `TransitionSync`.

Trong APK có endpoint:

`http://10.0.2.2:3000/api/v1/`

Đây là địa chỉ thường dùng để Android Emulator truy cập dịch vụ chạy trên máy host.

Manifest của bản (5) đã bổ sung:

`android.permission.INTERNET`

và cho phép cleartext HTTP phục vụ môi trường thử nghiệm.

**Nhận xét:** đây là bằng chứng rõ rằng quá trình kết nối frontend–backend qua `/api/v1` đã bắt đầu được đưa vào bản APK thực tế, chứ không còn chỉ nằm ở kế hoạch.

#### c. Đồng bộ dữ liệu

Các lớp `SyncCoordinator`, `SyncOutbox`, `PreferencesSyncStore` và `TransitionSync` cho thấy ứng dụng đã có cơ chế chuẩn bị hàng đợi và đồng bộ dữ liệu với backend.

**Nhận xét:** đây là hướng tốt đối với hệ thống cảnh báo vì dữ liệu cảm biến/sự kiện không nên phụ thuộc hoàn toàn vào một request đơn lẻ. Tuy nhiên vẫn cần kiểm thử runtime để xác nhận outbox có thực sự gửi lại dữ liệu sau khi mất mạng hay không.

#### d. Quản lí contacts

Bản (5) tiếp tục giữ các repository cục bộ và đồng thời có thêm DTO/API cho contacts.

**Nhận xét:** cấu trúc đã tiến gần tới mục tiêu quản lí danh sách người thân ở Android và đồng bộ qua backend.

#### e. Những chức năng chưa thể xem là hoàn thiện

Dù mã nguồn đóng gói đã có nhiều chuỗi/lớp liên quan tới SOS, location và contact, manifest của bản (5) **chưa phát hiện các quyền quan trọng**:

- `ACCESS_FINE_LOCATION` / `ACCESS_COARSE_LOCATION`.
- `SEND_SMS`.
- `CALL_PHONE`.
- `BLUETOOTH_CONNECT`.
- `BLUETOOTH_SCAN`.

Do đó tại thời điểm của APK (5):

1. Chưa có đủ bằng chứng để xác nhận ứng dụng có thể tự lấy vị trí GPS theo luồng cứu hộ đã thiết kế.
2. Chưa có đủ quyền để gửi SMS trực tiếp bằng ứng dụng.
3. Chưa có quyền để thực hiện cuộc gọi trực tiếp theo cơ chế cần quyền `CALL_PHONE`.
4. Chưa có quyền Bluetooth hiện đại để kết nối ESP32 trên các Android mới.
5. Phần giao thức ESP32 đã tồn tại nhưng chưa thể kết luận kết nối Bluetooth thật đã hoàn thiện.

Đây không nhất thiết là lỗi nếu các chức năng trên chưa được triển khai ở thời điểm build bản (5); nó phản ánh đúng trạng thái phát triển hiện tại.

### 13.7. Tổng hợp tiến trình qua các APK

| Phiên bản | Thay đổi chính quan sát được |
|---|---|
| `app-debug.apk` | Nền tảng cảm biến điện thoại, monitoring service, detector thử nghiệm, decoder ESP32 |
| `(1)` | Thiết kế lại màn hình chính/HomeScreen, bổ sung rung và UI hành động trung tâm |
| `(2)` | Thêm quản lí người thân, ContactsScreen, repository và lưu contacts cục bộ |
| `(3)` | Trùng hoàn toàn với `(2)` |
| `(4)` | Không lưu vì bản tải nhầm/trùng `(3)` |
| `(5)` | Thêm calibration/ngưỡng té ngã, lưu nhiều profile, REST API `/api/v1`, backend sync, contacts API |

### 13.8. Kết luận kiểm thử APK đợt 1

Chuỗi APK cho thấy phần mềm đã phát triển theo đúng hướng của nhật kí dự án:

**cảm biến thử nghiệm → thiết kế UI → quản lí người thân → hiệu chỉnh ngưỡng → kết nối backend.**

Bản (5) là bản có kiến trúc hoàn chỉnh nhất trong số các file được cung cấp, nhưng **chưa nên gọi là bản hoàn thiện chức năng cứu hộ**, vì các quyền và thành phần cần thiết cho vị trí, SMS, cuộc gọi và Bluetooth vẫn chưa hiện diện đầy đủ trong APK.

Các bước kiểm thử tiếp theo cần thực hiện trên thiết bị Android thật hoặc một Android Emulator đầy đủ:

1. Cài từng APK và chụp lại màn hình chính để đối chiếu tiến trình UI.
2. Kiểm tra crash bằng `adb logcat`.
3. Kiểm tra dữ liệu accelerometer/gyroscope/barometer.
4. Kiểm tra lưu, tải lại và chuyển giữa các profile ngưỡng.
5. Kiểm tra contacts sau khi đóng/mở lại ứng dụng.
6. Kiểm tra API `/api/v1` khi backend hoạt động.
7. Kiểm tra ứng dụng khi backend mất kết nối và khi kết nối trở lại.
8. Sau khi bổ sung quyền và implementation tương ứng, kiểm tra GPS, SMS, cuộc gọi SOS và Bluetooth ESP32 trên điện thoại thật.



## 14. Bổ sung quyền Android và hoàn thiện luồng SOS — 17–18/09/2026

Trong quá trình kiểm tra ứng dụng trên thiết bị thật, tiếp tục hoàn thiện các quyền và chức năng phục vụ cảnh báo khẩn cấp:

- Rà soát và xử lí luồng xin quyền runtime liên quan đến vị trí, SMS và cuộc gọi.
- Kiểm tra lại các nhánh SOS sau khi bổ sung quyền; kết quả ghi nhận tại thời điểm đó là **7/7 kiểm tra chấp nhận đạt** và **149 unit tests đạt**.
- Sửa nhận diện khả năng gọi điện và bổ sung chẩn đoán runtime cho chức năng điện thoại. Thay đổi này được ghi nhận trong commit `2b0eacc` (fix Android telephony capability detection and runtime diagnostics).

Các kết quả trên xác nhận những bài kiểm tra đã chạy ở thời điểm đó; chúng không thay thế cho việc tiếp tục thử trên điện thoại thật với SIM, GPS và các tình huống mất mạng khác nhau.

## 15. Kiểm chứng SOS, vị trí và ghi dữ liệu cảm biến — 19–20/09/2026

### 15.1. Kiểm thử hồi quy

- Bộ kiểm thử Android đạt **206/206 tests**, gồm **30 test suites**, không có test thất bại, lỗi hoặc test bị bỏ qua.
- Kiểm thử LOC-01 đạt **6/6**; kiểm thử hồi quy SOS của repository đạt **2/2**.
- Kết quả được chạy lại trên APK vừa build sau các thay đổi liên quan, rồi đối chiếu với trạng thái mã nguồn. Đây là kết quả kiểm thử phần mềm; không được hiểu là mọi tình huống ngoài thực địa đã được kiểm chứng.

### 15.2. Ghi vết FALL-01

Bổ sung logger để ghi dữ liệu gia tốc, con quay hồi chuyển và trạng thái phát hiện nhằm điều tra các trường hợp hệ thống chưa nhận ra tình huống ngã. Ở thời điểm ghi nhận, mã logger đã được chỉnh sửa và APK đã build, nhưng người nghiên cứu **chưa cài APK đó lên điện thoại để thử**. Vì vậy chưa có kết luận thực nghiệm rằng logger hoặc thay đổi phát hiện đã giải quyết được vấn đề.

### 15.3. Các vấn đề quan sát được cần tiếp tục kiểm tra

- Thử ngã xuống giường chưa kích hoạt cảnh báo trong tình huống đã báo cáo; cần thu log từ APK mới và phân biệt đây là giới hạn ngưỡng, tư thế thử hay lỗi trong luồng giám sát.
- Nếu bật GPS sau khi ứng dụng đã mở, ứng dụng chưa cập nhật vị trí như mong đợi trong lần thử được báo cáo. Cần kiểm tra lại quyền, trạng thái provider và cơ chế làm mới vị trí khi ứng dụng trở lại foreground.
- Người nghiên cứu chưa cài APK logger tại thời điểm trao đổi, nên các nhận xét trên là vấn đề còn mở, không phải kết quả đã xác nhận bằng bản build logger.

### 15.4. Việc tiếp theo

1. Cài APK có FALL-01 logger lên điện thoại thật và xác nhận dịch vụ cảm biến đang chạy.
2. Thực hiện các tình huống an toàn, ghi log cùng trạng thái ứng dụng và đối chiếu thời điểm cảnh báo.
3. Bổ sung vùng chẩn đoán trong **Cài đặt** để hiển thị giá trị cảm biến trực tiếp, giúp xác nhận ứng dụng có đang nhận dữ liệu hay không.
4. Thử lại luồng bật GPS sau khi mở ứng dụng và kiểm tra việc cập nhật vị trí trước khi gửi SOS.
5. Chỉ điều chỉnh ngưỡng sau khi đã xem dữ liệu ghi được; lưu rõ cấu hình và kết quả của từng lượt thử.
