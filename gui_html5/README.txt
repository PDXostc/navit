Faurecia HMI demostrator
========================

Welcome to the project. It is very early stage of this project so information
put here will be dynamically evolving. Stay tuned.

TODO:
-----

1. Write about how to contribute
2. Write about build system tasks
3. Write about common pitfalls


Scope
-----


Testing tools
-------------

So far there is no decision on how to test or to test what so ever. However if testing
is the case, Jasmine + Karma are suggested technologies to be used.

Static code analysis 
--------------------

Although there is no mentioning about it, JSHint or JSLint is suggested.


Installation
============

Install Node.js and npm package manager for node.

Next clone repository
> git clone git@gitlab.mobica.com:navit-gui/navit-gui.git
> cd navit-gui

Then install all dependencies
> sudo npm -g install grunt-cli karma bower
> npm install
> bower install


Architecture
============

Short overview of application architecture and build system remarks.

Directory structure
-------------------

The `src/` directory contains all code used in the application along with all
tests of such code.

```
src/
  |- app/
  |  |- about/
  |  |- home/
  |  |- app.js
  |  |- app.spec.js
  |- assets/
  |- common/
  |  |- plusOne/
  |- less/
  |  |- main.less
  |  |- variables.less
  |- index.html
```

----------------------
The `index.html` file
----------------------

The `index.html` file is the HTML document of the single-page application (SPA)
that should contain all markup that applies to everything in the app, such as
the header and footer. It declares with `ngApp` that this is `navit-gui`,
specifies the main `AppCtrl` controller, and contains the `ngView` directive
into which route templates are placed.

Unlike any other HTML document (e.g. the templates), `index.html` is compiled as
a Grunt template, so variables from `Gruntfile.js` and `package.json` can be
referenced from within it. Changing `name` in `package.json` from
"navit-gui" will rename the resultant CSS and JavaScript placed in `build/`,
so this HTML references them by variable for convenience.

-------------------------
The `src/app/` directory
-------------------------

The `src/app` directory contains all code specific to this application. Apart
from `app.js` and its accompanying tests (discussed below), this directory is
filled with subdirectories corresponding to high-level sections of the
application, often corresponding to top-level routes. Each directory can have as
many subdirectories as it needs, and the build system will understand what to
do. For example, a top-level route might be "products", which would be a folder
within the `src/app` directory that conceptually corresponds to the top-level
route `/products`, though this is in no way enforced. Products may then have
subdirectories for "create", "view", "search", etc. The "view" submodule may
then define a route of `/products/:id`, ad infinitum.

------------------
The `app.js` file
------------------

This is our main app configuration file. It kickstarts the whole process by
requiring all the modules from `src/app` that we need. We must load these now to
ensure the routes are loaded. If as in our "products" example there are
subroutes, we only require the top-level module, and allow the submodules to
require their own submodules.

As a matter of course, we also require the template modules that are generated
during the build.

However, the modules from `src/common` should be required by the app
submodules that need them to ensure proper dependency handling. These are
app-wide dependencies that are required to assemble your app.

```js
angular.module( 'navitGui', [
  'templates-app',
  'templates-common',
  'navitGui.home',
  'navitGui.about'
  'ui.router',
  'ui.route'
])
```

With app modules broken down in this way, all routing is performed by the
submodules we include, as that is where our app's functionality is really
defined.  So all we need to do in `app.js` is specify a default route to follow,
which route of course is defined in a submodule. In this case, our `home` module
is where we want to start, which has a defined route for `/home` in
`src/app/home/home.js`.

```js
.config( function myAppConfig ( $stateProvider, $urlRouterProvider ) {
  $urlRouterProvider.otherwise( '/home' );
})
```

Use the main applications run method to execute any code after services
have been instantiated.

```js
.run( function run () {
})
```

And then we define our main application controller. This is a good place for logic
not specific to the template or route, such as menu logic or page title wiring.

```js
.controller( 'AppCtrl', function AppCtrl ( $scope, $location ) {
  $scope.$on('$stateChangeSuccess', function(event, toState, toParams, fromState, fromParams){
    if ( angular.isDefined( toState.data.pageTitle ) ) {
      $scope.pageTitle = toState.data.pageTitle + ' | navitGui' ;
    }
  });
})
```

-----------------------------
The `src/app/home` directory
-----------------------------

```
src/
  |- app/
  |  |- home/
  |  |  |- home.js
  |  |  |- home.less
  |  |  |- home.spec.js
  |  |  |- home.tpl.html
```

- `home.js` - defines the module.
- `home.less` - module-specific styles; this file is imported into
  `src/less/main.less` manually by the developer.
- `home.spec.js` - module unit tests.
- `home.tpl.html` - the route template.

-------------------
The `home.js` file
-------------------

This boilerplate is too simple to demonstrate it, but `src/app/home` could have
several sub-folders representing additional modules that would then be listed
as dependencies of this one.  For example, a `note` section could have the
submodules `note.create`, `note.delete`, `note.search`, etc.

Regardless, so long as dependencies are managed correctly, the build process
will automatically take take of the rest.

The dependencies block is also where component dependencies should be
specified, as shown below.

```js
angular.module( 'navitGui.home', [
  'ui.router',
  'titleService'
])
```

Each section or module of the site can also have its own routes. AngularJS will
handle ensuring they are all available at run-time, but splitting it this way
makes each module more self-contained.

We use [ui-router](https://github.com/angular-ui/ui-router) to create a state
for our 'home' page. We set the url we'd like to see in the address bar as well as
the controller and template file to load. Specifying "main" as our view means the
controller and template will be loaded into the <div ui-view="main"/> element
of the root template (aka index.html).
Read more over at the [ui-router wiki](https://github.com/angular-ui/ui-router/wiki).
Finally we add a custom data property, pageTitle, which will be used to set the page's
title (see the app.js controller).

```js
.config(function config( $stateProvider ) {
  $stateProvider.state( 'home', {
    url: '/home',
    views: {
      "main": {
        controller: 'HomeCtrl',
        templateUrl: 'home/home.tpl.html'
      }
    },
    data:{ pageTitle: 'Home' }
  });
})
```

And of course we define a controller for our route, though in this case it does
nothing.

```js
.controller( 'HomeCtrl', function HomeController( $scope ) {
})
```

---------------------------
The `src/assets` directory
---------------------------

There's really not much to say here. Every file in this directory is recursively
transferred to `dist/assets/`.

------------------------------
The `src/common/` directory
------------------------------

The `src/common/` directory houses internal and third-party re-usable
components. Essentially, this folder is for everything that isn't completely
specific to this application.

Each component resides in its own directory that may then be structured any way
the developer desires. The build system will read all `*.js` files that do not
end in `.spec.js` as source files to be included in the final build, all
`*.spec.js` files as unit tests to be executed, and all `*.tpl.html` files as
templates to compiled into the `$templateCache`. There is currently no way to
handle components that do not meet this pattern.

Every component contained here should be drag-and-drop reusable in any other 
project; they should depend on no other components that aren't similarly 
drag-and-drop reusable.

---------------------------
The `src/less` directory
---------------------------

This folder is actually fairly self-explanatory: it contains your LESS/CSS files
to be compiled during the build. The only important thing to note is that *only*
`main.less` will be processed during the build, meaning that all other stylesheets
must be *imported* into that one.

This should operate somewhat like the routing; the `main.less` file contains all
of the site-wide styles, while any styles that are route-specific should be imported
into here from LESS files kept alongside the JavaScript and HTML sources of that
component. For example, the `home` section of the site has some custom styles, which
are imported like so:

```css
@import '../app/home/home.less';
```

The same principal, though not demonstrated in the code, would also apply to
reusable components. CSS or LESS files from external components would also be
imported. If, for example, we had a Twitter feed directive with an accompanying
template and style, we would similarly import it:

```css
@import '../common/twitterFeed/twitterFeedDirective.less';
```

Using this decentralized approach for all our code (JavaScript, HTML, and CSS)
creates a framework where a component's directory can be dragged and dropped into
*any other project* and it will "just work".

========================
Testing the application
========================

Our approach is that tests should exist alongside the code they test and that
the build system should be smart enough to know the difference and react
accordingly. As such, the unit test for `app.js` is `app.spec.js`, though it is
quite minimal.