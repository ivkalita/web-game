define(['backbone'], function(Backbone){
    var router = new Backbone.Router();

    var MenuItemModel = Backbone.Model.extend({
        initialize: function(){
            if (!this.get('link'))
                this.set('link', this.get('route'));
        },
        defaults: {
            route: null,
            link: null,
            name: null,
            caption: null,
            showAnonymous: true,
            showRegistered: true,
            active: false,
            position: 'left'
        },
        sync: function() { return false; }
    });

    var MenuItemCollection = Backbone.Collection.extend({
        model: MenuItemModel,
        setActive: function(s) {
            this.each(function(i){ i.set('active', s.startsWith(i.get('link'))); });
        }
    });

    return new (Backbone.Model.extend({
        initialize: function(){
            router.bind('route', function(route){
                this.set('state', window.location.hash.substr(1));
            }, this);

            this.bind('change:state', function() {
                var state = this.get('state');
                router.navigate(state, {trigger: false});
                this.get('menu').setActive(state);
            });

            if (localStorage['token'])
                this.set('token', localStorage['token']);
            this.bind('change:token', function(){
                if (this.get('token'))
                    localStorage.setItem('token', this.get('token'));
                else
                    localStorage.removeItem('token');
            });
        },

        defaults: {
            user: null,
            state: null,
            token: null,
            userName: null,
            menu: new MenuItemCollection
        },

        route: function(options){
            router.route(options.route, options.name, options.handler);
            if (!options.caption) return;
            this.get('menu').create(options);
        },

        applyView: function applyView(view) {
            $('main').empty().append(view.render().el);
        }
    }));
});
