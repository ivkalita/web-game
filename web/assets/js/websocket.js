define(['app-state'], function(appState) {
    if (!('WebSocket' in window))
        return alert('This browser does not support WebSockets.');

    var socket, handlers = {}, connectionCount = 0;

    function open() {
        socket = new WebSocket('ws://' + location.hostname + ':1337/');

        socket.onmessage = function(evt) {
            console.log('WebSocket reserve: ' + evt.data);
            var msg = JSON.parse(evt.data);
            if (msg.result == 'ok' && msg.action) {
                if (handlers[msg.action])
                    handlers[msg.action].handler(msg.data);
                else
                    alert('Отсутствует обработчик действия "' + msg.action + '"');
            } else {
                var message = {
                    'notFound': 'Действие не найдено',
                    'needAuth': 'Для действия требуется авторизация',
                    'badRequest': 'Неправильный запрос',
                    'internalError': 'Произошла внутренняя ошибка сервера'
                }[msg.result];
                message = message ? message : 'Неизвестная ошибка';
                if (handlers[msg.action].error)
                    handlers[msg.action].error(message);
                else
                    alert(message);
                if (handlers[msg.action].type == 'request')
                    handlers[msg.action] = undefined;
            }
        };

        socket.onopen = function() {
            console.log('WebSocket: opened');
            connectionCount++;
            if (appState.get('token')) {
                request('user/token', {token: appState.get('token')}, function(data){
                    if (data.result == 'badToken') {
                        appState.set('token', null);
                        appState.set('user', null);
                    } else {
                        appState.set('user', data.name);
                    }
                });
            }
        };

        socket.onclose = function() {
            if (connectionCount < 50) {
                console.log('WebSocket: closed, try to reconnect');
                open();
            } else alert('Проблема с подключением к серверу, обновите страницу');
        };
    }
    open();

    function send(action, data){
        var msg = JSON.stringify({
            action: action,
            data: data
        });
        socket.send(msg);
        console.log('WebSocket send: ' + msg);
    }

    function register (action, handler){
        handlers[action] = {
            handler: handler
        };
    }

    function request(action, data, handler, error){
        send(action, data);
        handlers[action] = {
            handler: handler,
            error: error,
            type: 'request'
        };
    }

    var f = {
        send: send,
        register: register,
        request: request
    };

    return f;
});
