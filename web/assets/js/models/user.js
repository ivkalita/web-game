define(['backbone', 'websocket'], function(Backbone, websocket) {
    return Backbone.Model.extend({
        defaults: {
            login: null,
            password: null,
            name: null,
            token: null
        },
        validate: function(attributes) {
            var r = {};
            if (!attributes.login) r.login = 'Логин не может быть пустым';
            if (!attributes.login) r.login = 'Пароль не может быть пустым';
            if (!attributes.login) r.login = 'Имя пользователя не может быть пустым';
            if (Object.keys(r).length) return r;
        },
        sync: function(method, model, options) {
            if (method != 'create') return false;
            websocket.send('user/register', {
                login: model.get('login'),
                password: model.get('password'),
                name: model.get('name')
            });
            return new Promise(function(resolve, reject){
                websocket.register('user/register', function(data) {
                    if (data.result == 'badCredentials') {
                        reject('Неверное имя пользовалетя или пароль');
                        return;
                    }
                    this.set('name', data.name);
                    resolve();
                });
            });
        },
        
    });
});
