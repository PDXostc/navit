/**
 * Tests sit right alongside the file they are testing, which is more intuitive
 * and portable than separating `src` and `test` directories. Additionally, the
 * build process will exclude all `.spec.js` files from the build
 * automatically.
 */
describe( 'settings section', function() {
  beforeEach( module( 'navitGui.settings' ) );

  it( 'should have a dummy test', inject( function() {
    expect( true ).toBeTruthy();
  }));
});
