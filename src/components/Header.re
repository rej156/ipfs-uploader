let component = ReasonReact.statelessComponent("Header");

let make = (~siteTitle: string, _children) => {
  ...component,
  render: _self =>
    MaterialUi.(
      <div>
        <AppBar position=`Static color=`Primary>
          <Toolbar>
            <Typography variant=`H3>
              {ReasonReact.string(siteTitle)}
            </Typography>
          </Toolbar>
        </AppBar>
      </div>
    ),
};