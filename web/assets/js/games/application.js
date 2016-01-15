define([
    'jquery', 
    'backbone', 
    'routers/router', 
    'views/game_view', 
    'views/games_view', 
    'views/form_view',
    'views/room_view',
    'models/game', 
    'models/games',
    'models/messages'
    ], function($, Backbone, Router, GameView, GamesView, FormView, RoomView, Game, Games, Messages) {

    var Application = (function() {
        var router, views = {}, models = {}, event;


        var module = function() {};

        module.prototype = {
            constructor: module,

            init: function() {
                this.initRouter();
                this.initModel();
                this.initView();
            },

            initRouter: function() {
                router = new Router();
                Backbone.history.start();
            },

            initView: function() {
                views.games = new GamesView({collection: models.games});
                views.form = new FormView({collection: models.games, router: router});
                views.room = new RoomView();

                views.games.render();
                views.form.render();
                views.room.render();
            },

            initModel: function() {
                models.games = new Games();
            }
        }

        return module;
    })();


    return Application;
});
