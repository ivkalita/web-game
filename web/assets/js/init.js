require(['require-config'], function() {
    require(['jquery',
        'app-state', 'views/menu',
        'collections/game', 'views/game/create', 'views/game/detail', 'views/game/list',
        'views/user/login', 'views/user/register', 'views/user/settings', 'views/user/logout'
        ],
        function($, appState, MenuView, GameCollection, GameCreateView, GameDetailView, GameListView) {
        $(document).ready(function() {

            var games = new GameCollection();

            appState.route({
                route: 'games(/)',
                link: 'games',
                name: 'game_list',
                handler: function () {
                    var view = new GameListView({collection: games});
                    appState.applyView(view);
                },
                caption: 'Игры'
            });

            appState.route({
                route: 'games/create',
                name: 'game_create',
                handler: function () {
                    var view = new GameCreateView();
                    appState.applyView(view);
                },
                caption: 'Создать игру',
                showAnonymous: false
            });

            appState.route({
                route: 'games/:id',
                name: 'game_detail',
                handler: function(id){
                    var view = new GameListView();
                    appState.applyView(view);
                }
            });

            var menu = new MenuView({model: appState});
            $('.navbar .container').append(menu.render().el);

            Backbone.history.start();

            if (window.location.hash == '')
                window.location.hash = '#games';
        })
    });
});
