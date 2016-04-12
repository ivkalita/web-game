define([
    'jquery', 'backbone', 'underscore', 'models/message'
], function($, Backbone, _, MessageModel) {
    return Backbone.View.extend({
        template: _.template($('#tpl-form').html()),
        initialize: function(options) {
            this.options = {};
            this.options.fields = options.fields;
            this.options.submitCaption = options.submitCaption;
            this.message = new MessageModel;

            this.listenTo(this.message, 'change', this.render);
            this.listenTo(this.model, 'change', this.render);

            this.listenTo(this.model, 'invalid', this.render);
            if (options.success) this.listenTo(this.model, 'sync', options.success);
            this.listenTo(this.model, 'sync', function(){
                this.message.set({
                    content: options.successMessage || 'Получен ответ от сервера',
                    type: MessageModel.Types.SUCCESS
                });
            });
            this.listenTo(this.model, 'error', function(model, message){
                this.message.set({
                    content: message,
                    type: MessageModel.Types.ERROR
                });
            });
            this.listenTo(this.model, 'request', function(){
                this.message.set({
                    content: 'Ожидание ответа сервера',
                    type: MessageModel.Types.WARNING
                });
            });
        },
        events: {
            'submit form': function(e) {
                e.preventDefault();
                var attributes = {};
                this.options.fields.forEach(function(option){
                    attributes[option.name] = $('input[name=' + option.name + ']').val()
                });
                this.model.set(attributes);
                this.model.save();
            }
        },
        render: function () {
            this.$el.html(this.template({
                fields: this.options.fields,
                submitCaption: this.options.submitCaption,
                object: this.model.toJSON(),
                errors: this.model.validationError,
                message: this.message.toJSON()
            }));
            return this;
        }
    });
});
