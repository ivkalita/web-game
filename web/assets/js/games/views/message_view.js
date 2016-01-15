define(['backbone'], function(Backbone) {

    var MessageView = Backbone.View.extend({
        tagName: 'div',

        template: _.template($('#tpl-message').html()),

        render: function() {
            this.$el.html(this.template(this.model.toJSON()));
            return this;
        }
    });


    return MessageView;
});
