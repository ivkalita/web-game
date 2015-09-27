# Client-server communication protocol

*****

# Table of contents
- [Serilization](#Serilization)
- [Classes](#Classes)
    - [Common](#Common)
        - [Response](#Response)
    - [Access control](#Access control)
        - [UserInfo](#UserInfo)
        - [Credentials](#Credentials)
    - [Connection with the game](#Connetction with the game)
        - [GameInfo](#GameInfo)
        - [Mode](#Mode)
        - [Map](#Map)
- [Methods](#Methods)
    - [Access control](#Access control)
        - [Authorization](#Authorization)
            - [authorize](#authorize)
        - [Registration](#Registration)
            - [register](#register)
        - [Logging out](#logging out)
            - [logout](#logout)
    - [Connection with the game](#Connection with the game)
        - [getGames](#getGames)
        - [joinToGame](#joinToGame)
        - [createGame](#createGame)
        - [startGame](#startGame)
        - [exitGame](#exitGame)

*****

# Serialization
Serialization of objects is performed in the format JSON. Read the rules of data in JSON format by this link: https://en.wikipedia.org/wiki/JSON#Data_types.2C_syntax_and_example 

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

## Connection with the game

### <a name="GameInfo"></a>GameInfo
```c++
struct GameInfo {
   int id;
   string name;
   UserInfo owner;
   int maxNumPeople;
   int curNumPeople;
   Mode mode;
   Map map;
}
```

### <a name="Mode"></a>Mode (TBD)
Type, which describes the mode of the game. Enum.

### <a name="Map"></a>Map (TBD)
Type, which describes the map of the game. Enum.

# Methods

## Access control
This section describes the methods of work with access control, such as authorization, registration and logging out.

### Authorization
Here are the methods of authorization.

#### authorize
This method should be used for authorization of users. Below you can see method signature.

```c++
Response authorize(Credentials credentials)
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
Here are the methods of registration.

#### register
This method should be used for registration of new users. Below you can see method signature.

```c++
Response register(string login, string name, string password)
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

#### logout
This method should be used for logging out of users. Below you can see method signature.
```c++
Response logout(void)
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


## Connection with the game
This section describes the methods which allow connect to specific game, as well as to get out of it.

#### getGames
This method allows you to get a list of all games (with information about them). Below you can see method signature.
```c++
Response getGames(void)
```
##### Arguments
There are no arguments

##### Result
[Response](#Response)::data contains array of [GameInfo](#GameInfo) objects with key "games" or nothing (in case of fail).
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - list of a games successfully obtained

##### Route
This method is available at url - **/api/get_games** [GET]


#### joinToGame (TBD)
This method allows to connect to specific game. Below you can see method signature.
```c++
Response joinToGame(UserInfo user, GameInfo game)
```
##### Arguments
[UserInfo](#UserInfo) user - required argument. User that is connected to the game.

[GameInfo](#GameInfo) game - required argument.

##### Result
TBD

##### Route
This method is available at url - **/api/join_to_game** [GET]


#### createGame
This method should be used for creating new game. Below you can see method signature.
```c++
Response createGame(UserInfo owner, GameInfo game)
```

##### Arguments
[UserInfo](#UserInfo) owner - required argument. The user who creates the game.

[GameInfo](#GameInfo) game - required argument. 

##### Result
TBD

##### Route
This method is available at url - **/api/create_game** [POST]


### startGame
This method allows the owner to start the game. Below you can see method signature.
```c++
Response startGame(UserInfo owner, GameInfo game)
```

##### Arguments
[UserInfo](#UserInfo) owner - required argument. The user who creates the game.

[GameInfo](#GameInfo) game - required argument.

##### Result
TBD

##### Route
This method is available at url - **/api/start_game** [POST]


### exitGame
This method allows the user to exit from the game. Below you can see method signature.
```c++
Response exitGame(UserInfo user, GameInfo game)
```

##### Arguments
[UserInfo](#UserInfo) user - required argument.

[GameInfo](#GameInfo) game - required argument.

##### Result
[Response](#Response)::data contains nothing.
[Response](#Response)::result may be one of follows or one of default [response results](#Response.result):
 * "Ok" - Quit the game there was a successfully.

##### Route
This method is available at url - **/api/exit_game** [POST]