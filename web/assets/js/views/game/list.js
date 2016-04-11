define(['backbone', 'underscore'], function(Backbone, _) {
    return Backbone.View.extend({
        template: _.template($('#tpl-game-list').html()),

        initialize: function() {
            // this.collection.bind('change', this.render);
        },

        render: function() {
            var items = $('<tbody></tbody>');

/*            this.collection.each(function(game) {
                var gameView = new GameView({model: game});
                items.append(gameView.render().el)
            }, this);*/

            this.$el.html(this.template({'gameList': items.html()}));
            return this;
        }
    });
});
