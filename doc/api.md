# Client-server communication protocol

*****

# Table of contents
- [Terminology](#terminology)
    - [Struct](#struct)
    - [Enum](#enum)
    - [Array](#array)
    - [Inheritance](#inheritance)
- [Serialization](#serialization)
- [Classes](#classes)
    - [Common](#common)
        - [AccessToken](#accesstoken)
        - [Data](#data)
        - [AccessTokenData](#accesstokendata)
        - [UserInfoData](#userinfodata)
        - [GameInfoData](#gameinfodata)
        - [Response](#response)
    - [Access control](#access-control)
        - [UserInfo](#userinfo)
        - [Credentials](#credentials)
    - [Game connection](#game-connection)
        - [Game](#game)
        - [GameInfo](#gameinfo)
        - [LobbyInfo](#lobbyinfo)
        - [Mode](#mode)
        - [Map](#map)
- [Methods](#methods)
    - [Access control](#access-control-1)
        - [Authorization](#authorization)
            - [Authorize](#authorize)
        - [Registration](#registration)
            - [Register](#register)
        - [Logging out](#logging-out)
            - [Logout](#logout)
    - [Game connection](#game-connection-1)
        - [GetGames](#getgames)
        - [JoinToGame](#jointogame)
        - [CreateGame](#creategame)
        - [StartGame](#startgame)
        - [LeaveGame](#leavegame)
        - [GetLobbyInfo](#getlobbyinfo)

*****

# Terminology
In this section some concepts and terms which occur in the protocol are explained.

## Struct
The term struct designates data type which contains variables of any types as properties. For example:
```c++
struct StructName {
    AnyType1 property1;
    AnyType2 property2;
    ...
    AnyTypeN propertyN;
}
```

## Enum
The term enum designates data type which contains the list of constants. For example:
```c++
enum EnumName {
    CONSTANT1;
    CONSTANT2;
    ...
    CONSTANTN;
}
```

## Array
To identify the variables that refer to the sequence data of any type used empty square brackets. For example:
```c++
anyType [] variableName
```

## Inheritance
Within this protocol inheritance like type "B" from the type "A" means that the type "B" contains the properties inherent in the type "A", and also some properties which can redefine type "A" properties. Such inheritance is designated as follows:
```c++
struct B: A {
    ...
}
```

# Serialization
Object serialization is performed in the JSON format. You can learn about data conversion to JSON format by following this link - https://en.wikipedia.org/wiki/JSON#Data_types.2C_syntax_and_example 

Table of correspondences of the types used in the protocol in the JSON types:
<table>
    <tr>
        <td>API type</td>
        <td>JSON type</td>
    </tr>
    <tr>
        <td>int</td>
        <td>number</td>
    </tr>
    <tr>
        <td>string</td>
        <td>string</td>
    </tr>
    <tr>
        <td>struct</td>
        <td>object</td>
    </tr>
    <tr>
        <td>enum</td>
        <td>string</td>
    </tr>
</table>


In case of POST requests objects of JSON are transferred with a key of **"jsonObj"**.

# Routing
For the appeal to methods from the section [Methods](#methods) the following template is used:
```
api/method_name
```

# Classes

## Common

### AccessToken
Object which identifies the user. **(TBD)**

### Data
This type is used in [Response](#responce).
```c++
struct Data { }
```

### AccessTokenData
```c++
struct AccessTokenData: Data {
    AccessToken accessToken;   
}
```

### UserInfoData
```c++
struct UserInfoData: Data {
    UserInfo user;
}
```

### GameInfoData
```c++
struct GameInfoData: Data {
   GameInfo [] game; 
}
```

### Response
```c++
struct Response {
    string result;
    string action;
    Data data;
}
```
Allowed values of **Response::result** specified for each method. Also this property can have the following values:
* "BadRequest"
* "InternalError"

## Access control

### UserInfo
```c++
struct UserInfo {
    int id;
    string login;
    string name;
}
```
#### Restrictions
for **UserInfo::login**:
```
[0-9a-zA-Z]{6, 36}
```
for **UserInfo::name**:
```
[a-zA-Z]{4, 32}
```

### Credentials
```c++
struct Credentials {
    string login;
    string password;
}
```
#### Restrictions 
for **Credentials::login**:
```
[0-9a-zA-Z]{6, 36}
```
for **Credentials::password**:
```
.{6, 36}
```

## Game connection

### Game
```c++
struct Game {
    string name;
    int maxNumPlayers;
    Mode mode;
    Map map;
}
```

### GameInfo
This type inherits [Game](#game).
```c++
struct GameInfo: GameInfo {
	int id;
    UserInfo owner;
    int curNumPlayers;
}
```
### LobbyInfo
This type inherits [GameInfo](#gameinfo) and [Data](#data).
```c++
struct GameInfo: GameInfo, Data {
	string[] players;
}
```

### Mode
Type, which describes the mode of the game. Enum. **(TBD)**

### Map
Type, which describes the map of the game. Enum. **(TBD)**

# Methods
**[Response](#Response)::action** is the name of the Method, which return it.

## Access control
This section describes access control methods, such as authorization, registration and logging out.

#### Authorize
This method available only for not authorized users. Its signature is shown below.

```c++
Response Authorize(Credentials credentials)
```

##### Result
**[Response](#Response)::data** contains object type of **[AccessTokenData](#accesstokendata)** (In case fail contains **NULL**).
**[Response](#Response)::result** may be one of follows or one of default [response results](#Response.result):
 * "Ok" - authorization successed
 * "BadCredentials" - user provides bad login/password

#### Register
This method available only for not authorized users. Its signature is shown below.

```c++
Response Register(string login, string name, string password)
```

##### Result
**[Response](#Response)::data** contains object type of **[UserInfoData](#userinfodata)** (In case fail contains **NULL**).
**[Response](#Response)::result** may be one of follows or one of default [response results](#Response.result):
 * "Ok" - registration successed
 * "BadPassword" - user provides bad password
 * "BadLogin" - user provides bad login
 * "LoginExists" - user provides existing login

#### Logout
This method should be used for logging users out. Its signature is shown below.
```c++
Response Logout(AccessToken accessToken)
```

##### Result
**[Response](#Response)::data** contains **NULL**.
**[Response](#Response)::result** may be one of follows or one of default [response results](#Response.result):
 * "Ok" - logging out succeed
 * "NotLoggedIn" - the user is not logged


## Game connection
This section describes methods of connection and disconnection to a specific game.

#### GetGames
This method allows you to get a list of all games (including game information). Its signature is shown below.
```c++
Response GetGames()
```

##### Result
**[Response](#Response)::data** contains object type of **[GameInfoData](#gameinfodata)** (In case fail contains **NULL**).
**[Response](#Response)::result** may be one of follows or one of default [response results](#Response.result):
 * "Ok" - list of a games successfully obtained


#### JoinToGame
This method allows to connect to specific game. Its signature is shown below.
```c++
Response JoinToGame(int gameId, AccessToken accessToken)
```
##### Arguments
int gameId - id of connected game.

##### Result
**[Response](#Response)::data** contains **NULL**.
**[Response](#Response)::result** may be one of follows or one of default [response results](#Response.result):
 * "Ok" - user is successfully connected to the game.
 * "NotFound" - this game is not exist.
 * "GameStarted" - this game already started.
 * "LobbyIsFull" - this game lobby is full.


#### CreateGame
This method should be used for creating new game. Its signature is shown below.
```c++
Response CreateGame(Game game, AccessToken accessToken)
```
##### Arguments
[Game](#Game) game - object with information about created game.

##### Result
**[Response](#Response)::data** contains **NULL**.
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - game successfully created.
 * "NotCreated" - failed to create game.

#### StartGame
This method allows the owner to start the game. Its signature is shown below.
```c++
Response StartGame(AccessToken accessToken)
```

##### Result
**[Response](#Response)::data** contains **NULL**.
**[Response](#Response)::result** may be one of follows or one of default [response results](#Response.result):
 * "Ok" - Start the game there was a successfully.
 * "NotStarted" - failed to start game.
 * "AlreadyStarted" - the game has already started.


#### LeaveGame
This method allows the user to exit the game. Its signature is shown below.
```c++
Response LeaveGame(AccessToken accessToken)
```

##### Result
**[Response](#Response)::data** contains **NULL**.
**[Response](#Response)::result** may be one of follows or one of default [response results](#Response.result):
 * "Ok" - Quit the game there was a successfully.
 * "OutOfGame" - Player out of any games.
 

#### GetLobbyInfo
This method should be used for getting lobby information. Its signature is shown below.
```c++
Response GetLobbyInfo(AccessToken accessToken)
```
##### Result
**[Response](#Response)::data** contains object type of **[LobbyInfo](#lobbyinfo)** (In case fail contains **NULL**).
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - lobby information successfully obtained.
 * "HaveNoLobby" - failed to get lobby information.
