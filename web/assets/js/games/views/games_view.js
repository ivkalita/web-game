define(['backbone', 'views/game_view'], function(Backbone, GameView) {

    var GamesView = Backbone.View.extend({
        el: $('#cont-game-list'),

        tpl_gameList: _.template($('#tpl-game-list').html()),
        tpl_noGames: _.template($('#tpl-no-games').html()),

        initialize: function() {
            _.bindAll(this, 'change');
            this.collection.bind('change', this.change);
        },

        render: function() {
            if (this.collection.length != 0) {
                var items = $('<tbody></tbody>');

                this.collection.each(function(game) {
                    var gameView = new GameView({model: game});
                    items.append(gameView.render().el)
                }, this);

                this.$el.html(this.tpl_gameList({'gameList': items.html()}));

            } else {
                this.$el.html(this.tpl_noGames());
            }

            return this;
        },

        change: function() {
            this.render();
        }
    });


    return GamesView;
});
