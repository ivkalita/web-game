define([
    'jquery', 'backbone', 'underscore'
], function($, Backbone, _) {
    return Backbone.View.extend({
        id: 'navbar',
        classList: ['collapse', 'navbar-collapse'],
        
        initialize: function() {
            this.model.bind('change', this.render, this);
        },
        template: _.template($('#tpl-menu').html()),
        render: function () {
            this.$el.html(this.template(JSON.parse(JSON.stringify(this.model))));
            return this;
        }
    });
});
