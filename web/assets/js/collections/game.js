define(['jquery', 'backbone', '../models/game'], function($, Backbone, Game) {
    return Backbone.Collection.extend({
        model: Game
    });
});
