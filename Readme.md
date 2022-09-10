# 1일 1커밋 목표

##### Inflearn에 Rookis님의 [C++과 언리얼로 만드는 MMORPG 게임 개발 시리즈](https://www.inflearn.com/course/%EC%96%B8%EB%A6%AC%EC%96%BC-3d-mmorpg-4/dashboard) 인터넷 강의를 공부하면서 작성했습니다.

- **2022.07.25** ServerCore files updated
  - mutex에서 에러가 발생. 멀티 쓰레드 환경 세팅과 메모리 관리 세팅을 다시 인강을 들으면서 공부할 필요가 있음.

- **2022.07.26** Mutex error 해결
  - CoreGlobal 객체를 생성하지 않은 것이 원인. 선언과 동시에 전역객체를 생성함으로써 콘텐츠단에서 생성하지 않아도 작동하도록 수정하였다.

- **2022.07.26** Reader/Writer Lock 추가
  - Lock을 더 편하게 사용하기 위한 라이브러리와 매크로이다.

- **2022.07.27** DeadLockProfiler 클래스 추가
  - DFS를 사용하여 Lock들 간의 Cycle이 있는지 판별하는 디버그용 클래스이다.
  - 여기가 첫번째 체크포인트

- **2022.07.29** SocketUtils와 NetAddress 클래스 추가
  - 윈도우 소켓 생성, 관리와 같은 관련된 함수를 wrapping한 SocketUtils 클래스와 나중에 accept될 세션들의 주소를 관리하는 NetAddress 클래스를 추가함.
  - 비동기 입출력 방식으로 사용하는 함수 포인터 ConnectEx와 AcceptEx, WSAIoctl() 등에 대해서 새롭게 배움.

- **2022.07.30** IocpCore, IocpEvent와 Listener 클래스 추가
  - Iocp와 관련된 함수를 담당하는 IocpCore와 세션의 상위 클래스 역할을 하는 IocpObject, Overlapped 구조체를 상속받아 대신 사용하년 IocpEvent 클래스를 추가함.
  - 이러한 클래스들을 사용하여 Accept하는 Listener 클래스도 추가함. 
  - IocpCore의 Register와 Dispatch, Listener의 RegisterAccept와 ProcessAccept 그리고 이벤트에 따른 IocpObject의 Dispatch까지의 이 흐름을 잘 이해해야 한다. 
  - 또한 추후에는 세션이 사라지지 않도록 Reference Couting을 해야함.

- **2022.07.31** IocpObject의 Reference Counting 수정
  - 이전에 작성한 코드에는 GQCS에서 키 값으로 복원한 IocpObject 포인터가 살아있는 상태인지 체크를 하지 않고 있다. 세션이 퇴장하여 누군가가 세션을 날려버린다고 가정하면 추후에 엉뚱한 주소에 접근하여 오류를 발생할 여지가 있다. 
  - 따라서 키 값을 사용하지 않고 Overlapped하여 사용하는 IocpEvent에 IocpObject shared_ptr을 멤버변수로 관리하여 이벤트의 주인이 누구인지 물고 있는 방법으로 수정함. 또한 shared_ptr을 좀 더 편하게 사용하기 위하여 using문을 추가함.

- **2022.07.31** Service 클래스 추가
  - 현재 IocpCore가 global하게 전역객체로 사용되고 있는데 경우에 따라 IocpCore를 여러 개 만들어 사용할 수도 있다. 분산서버를 만드는 경우가 그렇다. 
  - 또한, Listener가 accept를 할 때 Session을 생성하는데 실상은 콘텐츠단에서 Session을 정의하고 용도에 따라 여러 버젼의 Session을 생성할 수 있다. 
  - 그래서 위의 문제들을 해결하기 위해서 Client와 Server의 서비스를 나눌 수 있고 모든 Session을 관리하는 역할을 하는 Service 클래스를 추가함.
  - SessionFactory(function pointer)를 활용하여 인자를 넘겨받아 여러가지 버젼의 Session에 대해서도 대응할 수 있게 함. 
  - 사실 이러한 방법을 사용하지 않아도 된다. 다만 분산서버를 염두해서 미리 길을 만들어 놓은 듯 하다.

- **2022.08.01** Session 클래스 추가 #1
  - 각 소켓마다의 Recv와 Send를 담당할 Session 클래스를 추가함. 
  - 이 Session 클래스를 추후에 콘텐츠단에서 상속받아 GameSession과 같이 사용할 수 있기 때문에 엔진단에서만 사용할 네트워크 부분과 콘텐츠단에서 오버로딩하여 사용할 부분을 나누어서 설계해야한다. 
  - Register -> Process의 흐름을 잘 이해해야 한다. 
  - 세션에서 Service 객체를 물고 있는데 이 때 순환참조 되지 않도록 조심해야한다. weak_ptr을 사용함. 
  - 멤버변수로 RecvEvent를 들고 있는데 세션마다 하나씩 갖고 있고 매번마다 만들고 삭제하기 아깝기도 하며 session의 reference counting 용도로도 사용되기 때문에 멤버변수로 선언함. 이 ref counting을 잘 고려하여 코드를 설계해야한다. 
  - *현재 Recv 부분만 추가함.*

- **2022.08.03** Session 클래스 추가 #2

  - Session의 Send 관련 함수 부분들을 추가함. 
  - 하지만 Send는 Recv와 다르게 1번에 하나씩 실행되는 것이 아니라 여러 쓰레드에서 동시다발적으로 실행될 수 있기 때문에 멀티쓰레드에서 안전한지 고려해야 하고 그에 따른 버퍼 관리도 더 생각해야 한다. 
  - 또한 Broadcast시에 생기는 복사 비용에 대해서도 고려해야 하기 때문에 정책을 다르게 할 수 밖에 없다. 그래서 지금 임시로 구조를 잡아놨지만 수정해야 할 것이 많다. 
  - > TODO : recv한 데이터 중에서 아직 처리가 안된 부분을 건드리면 안되기 때문에 recvBuffer를 그대로 복사하는 부분은 수정이 필요함.

- **2022.08.04** Session 클래스 추가 #3

  - Disconnect 이벤트를 추가함. Connect와 Disconnect도 비동기 함수를 사용하여 Register -> Process 과정을 거치도록 함.  그 과정에서 socket을 재사용할 수 있도록 옵션을 정의함.
  - 또한, ClientService의 Start 함수도 정의하여 client가 서버에 연결을 시작할 때 Start 함수만 호출하면 session이 생성되고 각 session이 connect를 비동기적으로 호출하도록 함.

- **2022.08.31** RecvBuffer 클래스 추가

  - 기존에 임시로 사용하던 recvBuffer를 삭제하고 새로 RecvBuffer 클래스를 추가함. 
  - 매번 Recv할 때마다 수신 버퍼에 뒤집어 쓰는 것은 올바르지 않다. TCP의 특성 상 데이터의 바운더리 개념이 없기 때문에 수신된 버퍼가 완전하리라는 보장이 없기 때문이다. 그래서 매번 덮어쓰는 것이 아니라 읽기와 쓰기 커서를 두어 읽어야 할 데이터를 살려두고 덧붙여 쓸 수 있도록 한다. 
  - 또한 중요한 개념으로 버퍼 사이즈를 실제 사이즈보다 몇 배 이상 크게 함으로써 복사비용을 줄인다. 자세한 원리는 코드에 주석으로 설명함. 
  - 이제 만들어진 RecvBuffer와 커서들을 이용하여 실제 Recv에서 처리한 데이터의 사이즈와 비교하여 수신한 데이터의 유효성 검사도 추후에 처리 가능하다.

- **2022.09.01** SendBuffer 클래스 추가
  - Send가 Echo가 아닌 Broadcast하는 상황을 가정하였을 때, 매번 sendBuffer에 복사비용이 발생한다면 매우 무거운 작업이 될 것이다. 
  - 그래서 SendBuffer를 한번만 생성하여 데이터를 하나의 쓰레드에서 실행하도록 하면서 sendQueue에 밀어넣어준다. 이를 통해 Scatter-Gather 작업으로 데이터들을 모아서 한 방에 보내기 때문에 더 효율적으로 Send할 수 있다. 
  - 이렇게 하면 반응성에서는 떨어질 수 있지만, 효율성에서는 매우 좋기 때문에 MMO에서는 기준으로는 더 낫다고 볼 수 있다. 만약에 매 번마다 실제로 전송하는 경우는 sendEvent를 재사용하지 않고 매 번 새로 만들어야 한다. 
  - 또한 SendBuffer가 RegisterSend가 완료가 될 때까지는 사라지면 안되기 때문에 같이 RefCounting을 해주어야 한다.

- **2022.09.03** SendBufferChunk와 SendBufferManager 클래스 추가
  - SendBuffer를 할당할 사이즈를 미리 알 수가 없는 경우가 있다. 그럴 때는 콘텐츠단에서 얼만큼 할당해야 할까? 
  - 그래서 버퍼들을 필요할 때마다 한 번씩 할당 받지 않고 굉장히 큰 덩어리를 먼저 한번 할당 받은 후 그걸 쪼개서 사용하도록 함. 
  - 또한 SendBufferChunk를 각 쓰레드마다 TLS영역에서 하나씩 가지도록하여 멀티쓰레드의 경합을 피해 싱글 쓰레드처럼 작동하도록 함. 
  - 이 Chunk들을 재사용하기 위해서 Global한 영역에서 Push와 Pop을 하면서 메모리를 재사용하도록 관리해주는 SendBufferManager 클래스도 추가함.

- **2022.09.05** PacketSession 클래스 추가
  - TCP의 특성 상 Packet을 100Byte를 보냈다 한들 수신 측에서 온전히 수신할 거라는 보장이 없다.
  - 100Byte 중 20Byte만 수신할 수도 있다는 말이다. 그렇다면 Recv할 때 이 Packet이 온전한지는 어떻게 알 수 있을까? 
  - Packet의 Size와 Protocol ID를 담은 Packet Header를 붙여서 보내는 것이 일반적이다. PacketHeader를 Parsing하여 온전한 Packet을 수신했는지 알 수 있다. 
  - 만약 수신을 덜 했을 경우에는 다음 패킷이 도착할 때 까지 기다려줌으로써 패킷을 완전체로 조립한 다음에 콘텐츠단으로 넘겨주도록 한다. 이 로직을 다시 한번 살펴보도록 하자. 
  - 그리고 이 부분에서 서버는 해킹의 여지도 충분히 고려해야 한다. Packet의 Size와 ID를 확인하여 보안 이슈를 잘 신경 써야 한다. 
  - > DummyClient를 1000개로 하여 실행 한 후에 Client를 강제로 종료시키면 Server의 BroadCast에서 Crash가 나는데 그 이유는 Send를 할 때 Disconnect가 되어 수행할 때 OnDisconnect를 호출하여 for문을 도는 중에 erase를 실행하도록 로직이 짜여있기 때문이었다. 그래서 OnDisconnected가 ProcessDisconnect가 호출 될 때 실행하도록 수정함.

- **2022.09.06** BufferReader와 BufferWriter 클래스 추가
  - ServerCore 부분은 한 번 만들고 더 이상 건들지 않고, 주로 컨텐츠단에서 기능을 구현하게 될텐데 이 때 가장 중요한 부분이 패킷을 어떻게 만들어주고 전송할 것인가이다.
  - 주로 Google의 Protobuf를 많이 사용한다. 하지만 어떠한 옵션과 각각의 장단점을 이해하는 것이 좋기 때문에 직접 구현을 하면서 공부함.
  - 현재 버퍼를 만들 때 패킷헤더의 사이즈를 직접 계산하고 할당하는 것은 매우 번거롭고 실수의 여지가 있다. 그래서 버퍼를 읽고 쓰는데에 도움을 주는 클래스를 추가함.
  - 가변적인 길이의 데이터 같은 여러가지 프로토콜의 설계에 대해 고민해볼 필요가 있다.

- **2022.09.07** PacketHandler 클래스 추가

  - 게임에는 엄청 많은 종류의 프로토콜을 설계한다. 그리고 프로토콜별로 패킷을 파싱하는 방법이 다 다르다. 그래서 패킷을 파싱해 주는 새로운 ClientPacketHandler와 ServerPacketHandler 클래스를 추가함.

  - 가변 데이터에 대한 패킷을 파싱하는 방법을 알아봄. 그 중에서 문자열은 인코딩 이슈가 있음.

  - > 저번 시간에 만든 BufferWriter에서 왼값 오른값 참조하는 template 코드를 작성했는데 여기에서 문제가 발생함. template을 하면 보편참조가 되어서 왼값도 오른값 참조 오버로딩 함수로 들어오게 됨. 그래서 reference(&)의 포인터를 캐스트 하려고 하여 오류가 발생. 그래서 오류를 수정하였는데 왼값, 오른값과 template의 관계와 보편참조에 대해서 알아놔야 할 필요가 있다.

  - *중요한 마음가짐 : 클라이언트는 절대 신용할 수 없다. 패킷을 곧이곧대로 믿지 말자.*

- **2022.09.09** 문자열 패킷 파싱

  - 클라이언트와 통신할 때 똑같은 문자열을 주고 받기 위해서는 인코딩 규약을 맞출 필요가 있다. 아래처럼 문자 집합을 통일 시키기만 한다면 패킷을 파싱하는 방법은 똑같다.

  ```
  char sendData[1000] = "가"; // CP949 = KS-X-1001(한글2바이트) + KS-X-1003 (로마1바이트)
  char sendData2[1000] = u8"가"; // UTF8 = Unicode (한글3바티으 + 로마1바이트)
  WCHAR sendData3[1000] = L"가"; // UTF16 = Unicode (한글/로마 2바이트)
  TCHAR sendData4[1000] = _T("가"); // Template을 의미.  문자 집합을 자동으로 선택함.
  ```

  - 이 프로젝트에서는 WCHAR로 통일하기로 했다. 왜냐하면 공평하게 한글/로마 모두 2바이트이기 때문.
  
- **2022.09.10** 패킷 직렬화 #1

  - 파일을 입출력 할 때나 네트워크 통신을 할 때, 객체를 있는 그대로 보내게 되면 크래쉬가 난다. 왜냐하면 포인터나 배열 등과 같이 주소값들은 그대로 저장하거나 클라이언트에게 보내봤자 다시 불러오거나 클라이언트에서 읽을 때 엉뚱한 주소를 참조하게 되기 때문이다. 그래서 패킷 직렬화를 해야한다.
  - 먼저 직렬화하는 첫 번째 아이디어로 고정적인 데이터를 먼저 넣고 가변 데이터의 정보를 나타내는 헤더값을 offset과 count로 넣어줌. List 안의 List 같은 경우는 아직 고려하지 않음. 구글 FlatBuffers가 만들어지는 원리이기도 함.
  - 이 방법을 통해 고정적인 부분이 앞에 오고 가변적인 부분은 뒤에 오는데 offset으로 해당 데이터 위치를 따로 들고 있기 때문에 Validation을 바로 해줄 수 있다. 즉, 파싱이 빨라짐.

