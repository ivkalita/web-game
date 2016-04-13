define([
    'app-state', 'jquery', 'backbone', 'underscore', 'websocket', 'views/form'
], function(appState, $, Backbone, _, websocket, FormView) {
    appState.route({
        route: 'logout',
        name: 'logout',
        handler: function () {
            websocket.send('user/logout');
            appState.set('token', null);
            appState.set('user', null);
        },
        caption: 'Выйти',
        showAnonymous: false,
        position: 'right'
    });
});
