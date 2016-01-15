define(['jquery', 'backbone', 'models/message'], function($, Backbone, Message) {

    var Messages = Backbone.Collection.extend({
        model: Message,

        initialize: function() {
            this.on('add', this.onAdd);
        },

        onAdd: function(model) {
            model.set({id: this.length - 1});
        }
    });

    return Messages;
});
