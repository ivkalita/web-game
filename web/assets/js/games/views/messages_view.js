define(['backbone', 'views/message_view'], function(Backbone, MessageView) {

    var MessagesView = Backbone.View.extend({
        tpl_messageList: _.template($('#tpl-messages').html()),

        initialize: function() {
            _.bindAll(this, 'change');
            this.collection.bind('change', this.change);
        },

        render: function() {
            if (this.collection.length != 0) {
                var items = '';

                this.collection.each(function(message) {
                    var messageView = new MessageView({model: message});
                    items = messageView.render().$el.html() + items;
                }, this);

                this.$el.html(this.tpl_messageList({'messageList': items}));

            } else {
                this.$el.html(this.tpl_messageList({'messageList': ''}));
            }

            return this;
        },

        change: function() {
            this.render();
        }
    });


    return MessagesView;
});
