define(['backbone'], function(Backbone) {

    var GameView = Backbone.View.extend({
        tagName: 'tr',

        template: _.template($('#tpl-game-item').html()),

        render: function() {
            this.$el.html(this.template(this.model.toJSON()));
            return this;
        }
    });


    return GameView;
});
