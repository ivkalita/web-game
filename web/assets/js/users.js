function networkError() {
    alert('Произошла ошибка, требуется обновить страницу.');
}

var user = {
    login: function(login, name, token) {
        localStorage['login'] = login;
        localStorage['name'] = name;
        localStorage['token'] = token;
        $('#username').text(name);
        $('.hidden-onlogin').addClass('hidden');
        $('.visible-onlogin').removeClass('hidden');
    },
    logout: function() {
        localStorage.clear();
        $('.hidden-onlogin').removeClass('hidden');
        $('.visible-onlogin').addClass('hidden');
    },
    init: function() {
        if (localStorage['login'])
            this.login(localStorage['login'], localStorage['name'], localStorage['token'])
    },
    getToken: function() {
        return localStorage['token'];
    }
}

$(document).ready(function() {
    user.init();
    $(document).on('shown.bs.modal', function (e) {
        $('[autofocus]', e.target).focus();
    });
    $.ajaxSetup({
        type: "POST",
        cache: false,
        error: networkError,
        dataType: "json"
    });

    function checkPasswords() {
        var password = this.value;
        var valid = true;
        var inputs = $(this).parents('form').find('input[type=password]');
        inputs.each(function(id, dom){
            valid &= dom.value == password;
        });
        inputs.each(function(id, dom){
            dom.setCustomValidity(valid ? '' : 'Пароли не совпадают')
        });
    }
    $('.form-register input[type=password]').focus(checkPasswords).change(checkPasswords);

    $('#btn-logout').click(function() {
        $.ajax({
            url: '/api/logout',
            data: { jsonObj: JSON.stringify({ 'accessToken': user.getToken() }) },
            success: function(response) {
                switch (response.result) {
                    case 'Ok': break;
                    case 'NotLoggedIn': alert('Выход уже выполнен.'); break;
                    default: alert('Произошла неизвестная ошибка.'); break;
                }
                user.logout();
            }
        });
    });

    $('.form-login').submit(function(e) {
        e.preventDefault();
        login = $(e.target).find("input[name=login]").val();
        $.ajax({
            url: '/api/login',
            data: {
                jsonObj: JSON.stringify( {
                    'login': login,
                    'password': $(e.target).find("input[name=password]").val()
                } ) },
            success: function(response) {
                switch (response.result) {
                    case 'Ok':
                        user.login(login, response.data.name, response.data.accessToken);
                        e.target.reset();
                        $(e.target).find("button.close").click();
                        break;
                    case 'BadCredentials': alert('Неправильные имя пользователя или пароль.'); break;
                    default: alert('Произошла неизвестная ошибка.'); break;
                }
            }
        });
    });

    $('.form-register').submit(function(e) {
        e.preventDefault();
        login = $(e.target).find("input[name=login]").val();
        name = $(e.target).find("input[name=name]").val();
        password = $(e.target).find("input[name=password]").val();
        $.ajax({
            url: '/api/register',
            data: {
                jsonObj: JSON.stringify( {
                    'login': login, 'name': name, 'password': password
                } ) },
            success: function(response) {
                switch (response.result) {
                    case 'Ok':
                        user.login(login, name, response.data.accessToken);
                        e.target.reset();
                        $(e.target).find("button.close").click();
                        break;
                    case 'BadPassword': alert('Неправильный пароль.'); break;
                    case 'BadLogin': alert('Неправильный логин.'); break;
                    case 'LoginExists': alert('Логин уже существует.'); break;
                    default: alert('Произошла неизвестная ошибка.'); break;
                }
            }
        });
    });
});
