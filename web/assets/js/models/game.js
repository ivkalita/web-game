define(['backbone', 'collections/user'], function(Backbone, userCollection) {
    return Backbone.Model.extend({
        players: new userCollection(),
        validate: function(attrs) {
            if (attrs.name == 'плохое название')
                return 'название игры нам не нравится';
        }
    });
});
