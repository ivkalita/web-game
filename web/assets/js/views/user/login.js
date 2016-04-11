define([
    'app-state', 'jquery', 'backbone', 'underscore', 'websocket', 'views/form'
], function(appState, $, Backbone, _, websocket, FormView) {
    var UserLoginModel = Backbone.Model.extend({
        validate: function(attributes) {
            var r = {};
            if (!attributes.login) r.login = 'Логин не может быть пустым';
            if (!attributes.password) r.password = 'Пароль не может быть пустым';
            if (Object.keys(r).length) return r;
        },
        defaults: {
            login: null,
            password: null,
            name: null,
            token: null
        },
        sync: function(method, model, options) {
            if (method != 'create') return false;
            this.trigger('request', model);
            websocket.request('user/login', {
                login: model.get('login'),
                password: model.get('password')
            }, function(data){
                if (data.result == 'badCredentials') {
                    options.error('Неверное имя пользовалетя или пароль');
                    return;
                }
                model.set('token', data.token);
                model.set('name', data.name);
                options.success();
            }, options.error);
        }
    });

    var userLoginModel = new UserLoginModel;

    appState.route({
        route: 'login',
        name: 'login',
        handler: function () {
            var view = new FormView({
                submitCaption: 'Войти',
                fields: [
                    {name: 'login', caption: 'Логин', type: 'text'},
                    {name: 'password', caption: 'Пароль', type: 'password'}
                ],
                model: userLoginModel,
                success: function() {
                    appState.set('user', userLoginModel.get('name'));
                    appState.set('token', userLoginModel.get('token'));
                    window.history.back();
                }
            });
            appState.applyView(view);
        },
        caption: 'Войти',
        showRegistered: false,
        position: 'right'
    });
});
