# Advanced Depth Map Generation System

## Major goals
1. Superpixel과 spectral clustering에 기반한 image segmentation 수행
2. 각 image segment에서 목표 샘플 위치 배열 추출
3. 목표 샘플 위치를 깊이 측정 장비에 전달
4. 3으로부터 얻은 깊이 샘플로 전체 깊이 데이터 추정

## Milestones
1. Matlab의 Image segmentation 코드 OpenCV로 migrate
2. 기존 펌웨어(범위 스캔)를 태스크 큐 파싱하여 스케쥴에 따라 캡쳐하는 방식으로 업그레이드
3. 라즈베리 파이에 1에서 OpenCV로 구현한 프로그램 최적화
4. PC에서 TCP/IP 프로토콜을 통해 장비 제어하는 콘솔 라이브러리 작성 (dll 형태)
5. Raspberry Pi에서 로컬 IP로 접근 시 HTTPS/HTTP 프로토콜로 장비 상태 리포트 웹페이지 전송

## Structure
