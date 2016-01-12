define(['jquery', 'backbone', 'models/game'], function($, Backbone, Game) {

    var Games = Backbone.Collection.extend({
        model: Game,

        initialize: function() {
            this.on('add', this.onAdd);
        },

        onAdd: function(model) {
            model.set({id: this.length - 1});
        }
    });

    return Games;
});
