# SKVS 0.1.0 (Alpha Test Version 1)
* SKVS(SweetCase Key-Value Storage)는 In Memory Key-Value 데이터베이스로 키를 이용해서 데이터를 받거나 수정을 합니다.
* MultiThread 방식으로 동작을 하므로, 하나의 일을 하는 동안 다른 일을 할 수 있습니다.

## Complier
* G++ (GCC) 7.3.1 20180303 (Red Hat 7.3.1-5)
* STANDARD C++17

## Library
### Personal Library
* TCP Socket Function Wrapper For C++ 0.0.1
### External Library
* rapidjson 1.1.0
* google protocol buffer : libprotoc 3.9.0

## Config 설정 방법
### Cilent
* client 디렉토리로 들어간 다음 login.json을 이용해서 port, id, pswd를 수정할 수 있습니다.
* 단 json의 구조가 회손 될 경우 프로그램이 제대로 작동하지 않습니다.
* login.json
<pre><code>{
	"server" : {
		"ip" : "127.0.0.1",
		"port" : 8000
	},
	"user" : {
		"id" : "user",
		"paswd" : "12345678"
	}
}
</code></pre>
### Server
* server 디렉토리로 들어가면 account.json과 system_config.json을 이용해서 계정정보나, 시스템 설정을 변경할 수 있습니다.
* 마찬가지로 json 구조가 회손되면 프로그램이 정상적으로 작동하지 않습니다.
* account.json
<pre><code>{
    "account": [
        {
            "id": "root",
            "pswd": "123456780",
            "userlevel": "root"
        },
        {
            "id": "user",
            "pswd": "12345678",
            "userlevel": "client"
        }
    ]
}
</code></pre>
* system_config.json
	* Style은 독립형("STANDALONE")과 분산형("DISTRIBUTED")으로 나뉘며 이 버전에는 독립형만 구현되어 있습니다. 분산형은 차후에 구현할 예정입니다.
<pre><code>{
    "port": 8000,
    "style": "STANDALONE",
    "log": {
        "location": "log/"
    }
}
</code></pre>


## Data Structure
* 모든 데이터의 최소 단위이며 멤버 변수는 다음과 같습니다.
<pre><code>string data;
DataType dataType;
StructType structType;
d_size_t size;
</code></pre>
* Data는 DataElement의 값을 나타내고 datatype에 해당하는 자료형으로 데이터가 추출됩니다. (Library에서만 사용되는 기능입니다.)
* DataType은 string , number, float으로 나뉘며, number는 long, float은 double 입니다.
* StructType은 해당 구조체의 타입을 나타냅니다. 해당 키의 하위 데이터로 설정되어 있는 경우에는 StructType이 Element로 설정됩니다.
* size는 data의 크기를 나타냅니다.

## Basic
* 전형적인 Key-Value 형 데이터 구조입니다. 하나의 Basic Key는 하나의 value만 생성이 가능하며 수정이 불가능합니다.

## OneSet, MultiSet
![struc_set](https://sdkvs-readme.s3.ap-northeast-2.amazonaws.com/SetStruct.png)
* OneSet은 동일한 값을 입력할 수 없지만, MutliSet은 입력이 가능합니다.
* Set을 생성할 때 반드시 데이터타입을 설정해야 하며 그 이후에 입력되는 데이터들은 Set의 요구 데이터 타입과 매칭되어야 합니다.
* Set의 데이터들은 범위에 따른 삭제가 가능하지만 수정은 불가능합니다.

## StaticList
![struct_static_list](https://sdkvs-readme.s3.ap-northeast-2.amazonaws.com/SetStruct.png)
* Set과 구조가 거의 동일하지만 , STL-set으로 구현된 Set 구조체와는 달리, list 컨테이너로 구현되어있으며, 인덱스로 접근이 가능하고, 데이터 수정이 가능합니다. 단 Static계열의 구조체는 Set처럼 데이터타입을 설정해줘야 하며 그 데이터 타입과 동일한 데이터들이 입력이 됩니다.

## DynamicList
![struct_dynamic_list](https://sdkvs-readme.s3.ap-northeast-2.amazonaws.com/DynamicStruc3.png)
* StaticList와 달리 데이터타입이 String으로 고정되어 있는 대신, Child Link를 통해 하위 데이터를 설계할 수 있습니다.
* 데이터의 연결의 위해 Dynamic 계열의 데이터구조체는 linkMap이라는 hashmap 컨테이너를 가지고 있는 데 이는 데이터가 하나 씩 추가할 때마다 linkMap에서도 데이터가 하나 씩 추가됩니다. 단 자원 낭비를 막기 위해 복사본이 아닌 포인터(shared_ptr)를 사용하였습니다.

## StaticHashMap
![static_hashmap](https://sdkvs-readme.s3.ap-northeast-2.amazonaws.com/StaticHashMap.png)
* key-value로 구성되어 있는 hashmap은 해당 key에 들어가서 subkey를 검색하면 value가 추출됩니다.
* value는 수정이 가능 하지만 key는 현재 알고리즘의 문제로 수정이 불가능합니다.
* static 계열이므로 데이터타입을 설정해 주어야 하고 value가 이에 대응합니다.
* key는 DataElement가 아닌 순수 std::string으로 선언되어 있습니다.

## DynamicHashMap
![dynamic_hashmap](https://sdkvs-readme.s3.ap-northeast-2.amazonaws.com/DynamicHashMap.png)
* Dynamic 계열이므로 데이터타입이 string으로 고정되어 있고 하위 데이터 설정이 가능합니다.
* 사용자가 하위 데이터를 접근 할 때는 sub-key를 입력할  sub-key에 대응하는 value에 접근 한 다음에 value와 매칭되는 하위 데이터로 접근을 합니다.

# Data Linking
![linking](https://sdkvs-readme.s3.ap-northeast-2.amazonaws.com/Linker.png)
* Data Linking은 같은 공간에 있는 데이터 까리 연결을 해서 마치 Tree형 데이터 구조로 보이게 하는 기능입니다.
* Basic을 제외 한 나머지 데이터구조체들은 각각 ParentData Pointer를 가지고 있기 때문에 이를 이용해서 데이터 끼리 연결이 가능합니다.
* Child Data를 제외한 value의 연결은 shared_ptr를 사용하기 때문에 상위 데이터에서의 하위 데이터 연결 여부는 shared_ptr의 참조 갯수로 판별을 합니다. 위의 그래프 처럼 value는 자신을 포함해서 3번 이상 참조되었을 경우, Linking이 되어 있는 것으로 간주하고 Parent Data PTR과 연결이 끊어져 2번이 참조 된 경우를 Linking이 되어 있지 않은 것으로 간주됩니다.
* 하나의 Parent는 하나의 Child만 가질 수 있고 하나의 Child도 마찬가지로 하나의 Parent만 가질 수 있습니다.

# Command List
* 타입 플래그
  * struct-type
    * basic, oneset, multiset, staticlist, dynamiclist, statichashmap, dynamichashmap
  * data-type
    * string, number, float
  * condition
    * over, eover(equal and over), under, eunder(equal and under), equal, nonequal
    * ex) over 300 and equal 100 -> number, float
    * ex) >aaa, \*aaa, aaa\* -> string
  * condition-index
    * min-max
    * ex) 1-3, 0-4, 2
  * sort
    * 오름차순 : asen
    * 내림차순 : desn

## Account Command
|명령어|설명|
|--|---|
|useradd [new user] [new password] [set level] | 유저를 생성합니다. __ROOT__ 유저만 사용 가능하며, __client__ 와 __root__ 로 레벨설정을 합니다.|
|userdel [user]| 유저를 삭제합니댜.  __ROOT__ 만 사용할 수 있습니다.|
## Data Command
|명령어|설명|
|--|---|
|create [struct-type] [key] {data-type}|데이터 구조체를 생성합니다. data-type은 Set이나 Statc계열에서 데이터타입을 설정할때 사용합니다.|
|drop [key] |데이터 구조체를 삭제합니다. Parent Data가 없는 최상위 데이터만 삭제가 가능합니다.|
|insert [key] [new-value]|해당 구조체에 데이터를 입력합니다.|
|insert [key] [new-key] [new-value]| 데이터 키와 값을 입력합니다. hashmap에서만 사용합니다.|
|get {-i} [key] {condition} | 데이터 키와 값을 입력합니다. hashmap에서만 사용합니다. condition을 입력하지 않으면, 모든 데이터가 조회됩니다. -i를 입력하면 인덱스로 범위를 설정 할 수 있습니다.|
|set {-i} [key] [value or index] [new data] | 데이터 수정을 합니다. -i를 입력하면 인덱스로 설정합니다.|
|link [key] [value(sub-key)] [target-key] | key의 value(sub-key)로 target-key를 연결합니다. value(sub-key)는 Parent가 되고 target-key는 child가 됩니다.|
|unlink [key] [value(sub-key)] | value의 link를 해제합니다. |
|delete {-i} [key] [condition] | 데이터를 삭제합니다. -i를 입력하면 인덱스로 범위를 설정 할 수 있습니다.|
|getsize [key] | 해당 key의 데이터 크기를 검색합니다. 단위는 byte이며 hashmap은 sub-key까지 계산합니다.|
|getkey [key] | hashmap의 sub-key를 나열합니다|
|list|전체 데이터 리스트를 검색합니다. Parent가 없는 최상위 데이터는 이름 앞에 \*가 붙습니다.|
|sort [key] [sort condition] |key에 들어있는 데이터를 정렬합니다. List계열 컨테이너만 사용 가능합니다.|
|quit|프로그램을 종료합니다.|

* __TIP__ : Child Data의 접근은 명령어의 [key]부분에서 '.'을 이용해서 접근이 가능합니다. EX) key.child_value
