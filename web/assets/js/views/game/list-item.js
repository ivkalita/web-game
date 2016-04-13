define(['backbone', 'underscore'], function(Backbone, _) {
    return Backbone.View.extend({
        tagName: 'tr',
        template: _.template($('#tpl-matchmaking-item').html()),

        render: function() {
            this.$el.html(this.template(this.model.toJSON()));
            return this;
        }
    });
});
