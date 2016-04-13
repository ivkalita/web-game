define([
    'app-state', 'jquery', 'backbone', 'underscore', 'websocket', 'views/form'
], function(appState, $, Backbone, _, websocket, FormView) {
    var UserRegisterModel = Backbone.Model.extend({
        validate: function(attributes) {
            var r = {};
            if (!attributes.login) r.login = 'Логин не может быть пустым';
            if (attributes.password != attributes.password2) r.common = 'Пароли не совпадают';
            if (!attributes.password) r.password = 'Пароль не может быть пустым';
            if (!attributes.password2) r.password2 = 'Пароль не может быть пустым';
            if (!attributes.name) r.name = 'Имя пользователя не может быть пустым';
            if (Object.keys(r).length) return r;
        },
        defaults: {
            login: null,
            password: null,
            password2: null,
            name: null,
            token: null
        },
        sync: function(method, model, options) {
            if (method != 'create') return false;
            this.trigger('request', model);
            websocket.request('user/register', {
                login: model.get('login'),
                password: model.get('password'),
                name: model.get('name')
            }, function(data){
                if (data.errors) {
                    if (data.errors.indexOf('loginExists') != -1)
                        options.error('Пользователь с таким логином уже существует');
                    else
                        options.error('Неизвестная ошибка');
                    return;
                }
                model.set('token', data.token);
                options.success();
            }, options.error);
        }
    });

    var userRegisterModel = new UserRegisterModel;

    appState.route({
        route: 'register',
        name: 'register',
        handler: function () {
            var view = new FormView({
                submitCaption: 'Зарегистрироваться',
                fields: [
                    {name: 'login', caption: 'Логин', type: 'text'},
                    {name: 'name', caption: 'Имя', type: 'text'},
                    {name: 'password', caption: 'Пароль', type: 'password'},
                    {name: 'password2', caption: 'Повторите пароль', type: 'password'}
                ],
                model: userRegisterModel,
                success: function() {
                    appState.set('user', userRegisterModel.get('name'));
                    appState.set('token', userRegisterModel.get('token'));
                    window.history.back();
                }
            });
            appState.applyView(view);
        },
        caption: 'Зарегистрироваться',
        showRegistered: false,
        position: 'right'
    });
});
