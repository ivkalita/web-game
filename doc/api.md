# Client-server communication protocol

*****

# Table of contents
- [Terminology](#terminology)
    - [Struct](#struct)
    - [Inheritance](#inheritance)
- [Serialization](#serialization)
- [Classes](#classes)
    - [Common](#common)
        - [Data](#data)
        - [UserInfoData](#userinfodata)
        - [GameInfoAllData](#gameinfoalldata)
        - [Response](#response)
    - [Access control](#access-control)
        - [UserInfo](#userinfo)
        - [Credentials](#credentials)
    - [Game connection](#game-connection)
        - [GameInfo](#gameinfo)
        - [GameInfoAll](#gameinfoall)
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

*****

# Terminology
In this section some concepts and terms which occur in the protocol are explained.

## Struct
The term struct designates data type which comprises variables of any types as properties. For example:
```c++
struct StructName {
    AnyType1 property1;
    AnyType2 property2;
    ...
    AnyTypeN propertyN;
}
```

## Enum
The term enum designates data type which comprises the list of integer constants, each of which has a certain identifier. For example:
```c++
enum EnumName {
    CONSTANT1;
    CONSTANT2;
    ...
    CONSTANTN;
}
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
| API type  | JSON type |
| ------------- | ------------- |
| int  | number |
| string  | string |
| struct type  | object |
| enum type  | object |

In case of POST requests objects of JSON are transferred with a key of "jsonObj".

# Routing
For the appeal to methods from the section [Methods](#methods) the following template is used:
```
api/method_name
```

# Classes

## Common

### Data
This type is used in [Response](#responce).
```c++
struct Data { }
```

### UserInfoData
```c++
struct UserInfoData: Data {
    UserInfo user;
}
```

### GameInfoAllData
```c++
struct GameInfoAllData: Data {
   GameInfoAll * game; 
}
```

### Response
```c++
struct Response {
    string result;
    Data data;
}
```
* <a name="Response.result"></a>result - ("BadRequest"|"InternalError")

## Access control

### UserInfo
```c++
struct UserInfo {
    int id;
    string login;
    string name;
}
```

### Credentials
```c++
struct Credentials {
    string login;
    string password;
}
```
* login - [0-9a-zA-Z]{6, 36}
* password - .{6, 36}

## Game connection

### GameInfo
```c++
struct GameInfo {
   int id;
   string name;
   UserInfo owner;
   int maxNumPlayers;
   Mode mode;
   Map map;
}
```

### GameInfoAll
This type inherits [GameInfo](#gameinfo).
```c++
struct GameInfoAll: GameInfo {
   int curNumPlayers;
}
```

### Mode
Type, which describes the mode of the game. Enum. **(TBD)**

### Map
Type, which describes the map of the game. Enum. **(TBD)**

# Methods

## Access control
This section describes access control methods, such as authorization, registration and logging out.

### Authorization
Here are the methods of authorization.

#### Authorize
This method should be used for authorization of users. Its signature is shown below.

```c++
Response Authorize(Credentials credentials)
```
##### Arguments
[Credentials](#Credentials) credentials - required argument.

##### Result
[Response](#Response)::data contains object like [UserInfoData](#userinfodata).
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - authorization succeed
 * "BadCredentials" - user provides bad login/password

##### Route
This method is available at url - **/authorize** [POST]


### Registration
Here are the registration methods.

#### Register
This method should be used for new user(s) registration. Its signature is shown below.

```c++
Response Register(string login, string name, string password)
```
##### Arguments
string login - required argument.

string name - required argument.

string password - required argument.

##### Result
[Response](#Response)::data contains object like [UserInfoData](#userinfodata).
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - registration succeed
 * "BadPassword" - user provides bad password
 * "BadLogin" - user provides bad login
 * "LoginExist" - user provides existing login

##### Route
This method is available at url - **/register** [POST]


### Logging out
Here are the methods for logging out.

#### Logout
This method should be used for logging users out. Its signature is shown below.
```c++
Response Logout(void)
```
##### Arguments
There are no arguments

##### Result
[Response](#Response)::data contains object like [Data](#data).
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - logging out succeed
 * "NotLoggedIn" - the user is not logged

##### Route
This method is available at url - **/logout** [POST]


## Game connection
This section describes methods of connection and disconnection to a specific game.

#### GetGames
This method allows you to get a list of all games (including game information). Its signature is shown below.
```c++
Response GetGames(void)
```
##### Arguments
There are no arguments

##### Result
[Response](#Response)::data contains object like [GameInfoAllData](#data).
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - list of a games successfully obtained

##### Route
This method is available at url - **/api/get_games** [GET]


#### JoinToGame
This method allows to connect to specific game. Its signature is shown below.
```c++
Response JoinToGame(UserInfo user, GameInfoAll game)
```
##### Arguments
[UserInfo](#UserInfo) user - required argument. User that is connected to the game.

[GameInfoAll](#GameInfoAll) game - required argument.

##### Result
[Response](#Response)::data contains object like [Data](#data).
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - user is successfully connected to the game.
 * "NotFound" - this game is not exist.
 * "GameStarted" - this game already started.

##### Route
This method is available at url - **/api/join_to_game** [GET]


#### CreateGame
This method should be used for creating new game. Its signature is shown below.
```c++
Response CreateGame(UserInfo owner, GameInfo game)
```

##### Arguments
[UserInfo](#UserInfo) owner - required argument. The user who creates the game.

[GameInfo](#GameInfo) game - required argument. 

##### Result
[Response](#Response)::data contains object like [Data](#data).
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - game successfully created.
 * "NotCreated" - failed to create game.

##### Route
This method is available at url - **/api/create_game** [POST]


### StartGame
This method allows the owner to start the game. Its signature is shown below.
```c++
Response StartGame(UserInfo owner, GameInfoAll game)
```

##### Arguments
[UserInfo](#UserInfo) owner - required argument. The user who creates the game.

[GameInfoAll](#GameInfoAll) game - required argument.

##### Result
[Response](#Response)::data contains object like [Data](#data).
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - Start the game there was a successfully.
 * "NotStarted" - failed to start game.

##### Route
This method is available at url - **/api/start_game** [POST]


### LeaveGame
This method allows the user to exit the game. Its signature is shown below.
```c++
Response LeaveGame(UserInfo user, GameInfoAll game)
```

##### Arguments
[UserInfo](#UserInfo) user - required argument.

[GameInfoAll](#GameInfoAll) game - required argument.

##### Result
[Response](#Response)::data contains object like [Data](#data).
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - Quit the game there was a successfully.

##### Route
This method is available at url - **/api/exit_game** [POST]