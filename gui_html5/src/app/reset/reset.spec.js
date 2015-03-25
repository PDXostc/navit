describe( 'resetting navit to factory defaults', function() {
  beforeEach( module( 'navitGui.reset' ) );

  it( 'should have a dummy test', inject( function() {
    expect( true ).toBeTruthy();
  }));
});

