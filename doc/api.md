# Client-server communication protocol

*****

# Table of contents
- [Serialization](#serialization)
- [Classes](#classes)
    - [Common](#common)
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
        - [ExitGame](#exitgame)

*****

# Serialization
Object serialization is performed in the JSON format. You can learn about data conversion to JSON format by following this link - https://en.wikipedia.org/wiki/JSON#Data_types.2C_syntax_and_example 

# Classes

## Common

### <a name="Response"></a>Response
```c++
struct Response {
    string result;
    object *data;
}
```
* <a name="Response.result"></a>result - ("BadRequest"|"InternalError")

## Access control

### <a name="UserInfo"></a>UserInfo
```c++
struct UserInfo {
    int id;
    string login;
    string name;
}
```

### <a name="Credentials"></a>Credentials
```c++
struct Credentials {
    string login;
    string password;
}
```
* login - [0-9a-zA-Z]{6, 36}
* password - .{6, 36}

## Game connection

### <a name="GameInfo"></a>GameInfo
```c++
class GameInfo {
public:
   int id;
   string name;
   UserInfo owner;
   int maxNumPlayers;
   Mode mode;
   Map map;
}
```

### <a name="GameInfoAll"></a>GameInfoAll
This type inherits [GameInfo](#gameinfo).
```c++
class GameInfoAll : public GameInfo {
public:
   int curNumPlayers;
}
```

### <a name="Mode"></a>Mode (TBD)
Type, which describes the mode of the game. Enum.

### <a name="Map"></a>Map (TBD)
Type, which describes the map of the game. Enum.

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
[Response](#Response)::data contains [UserInfo](#UserInfo) object with key "user" or nothing (in case of fail).
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
[Response](#Response)::data contains [UserInfo](#UserInfo) object with key "user" or nothing (in case of fail).
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
[Response](#Response)::data contains nothing.
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
[Response](#Response)::data contains array of [GameInfoAll](#GameInfoAll) objects with key "games" or nothing (in case of fail).
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
[Response](#Response)::data contains nothing.
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
[Response](#Response)::data contains nothing.
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
TBD

##### Route
This method is available at url - **/api/start_game** [POST]


### ExitGame
This method allows the user to exit the game. Its signature is shown below.
```c++
Response ExitGame(UserInfo user, GameInfoAll game)
```

##### Arguments
[UserInfo](#UserInfo) user - required argument.

[GameInfoAll](#GameInfoAll) game - required argument.

##### Result
[Response](#Response)::data contains nothing.
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - Quit the game there was a successfully.

##### Route
This method is available at url - **/api/exit_game** [POST]