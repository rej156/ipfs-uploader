[@bs.module "../lib/init-uppy.js"] external initUppy: unit => unit = "default";

type state = {
  isLoggedIn: bool,
  threeBox: Js.Nullable.t(ThreeBox.threeBox),
};

type action =
  | SetLoggedIn(bool)
  | SetThreeBox(ThreeBox.threeBox);
let component = ReasonReact.reducerComponent("App");

type data = {. "site": {. "siteMetadata": {. "title": string}}};

let meta = [|
  Helmet.metaField(~name="description", ~content="Sample"),
  Helmet.metaField(~name="keywords", ~content="sample, something"),
|];

let make = (~data, _children) => {
  ...component,
  initialState: () => {isLoggedIn: false, threeBox: Js.Nullable.undefined},
  reducer: (action, state) =>
    switch (action) {
    | SetLoggedIn(status) =>
      ReasonReact.Update({...state, isLoggedIn: status})
    | SetThreeBox(threeBox) =>
      ReasonReact.Update({...state, threeBox: Js.Nullable.return(threeBox)})
    },
  render: self =>
    <div>
      <Helmet title=data##site##siteMetadata##title meta>
        <link
          href="https://transloadit.edgly.net/releases/uppy/v0.27.5/dist/uppy.min.css"
          rel="stylesheet"
        />
      </Helmet>
      <Header siteTitle=data##site##siteMetadata##title />
      <BrowserWeb3Capabilities
        isLoggedIn={self.state.isLoggedIn} loggedInAddress="123456789">
        ...{_ => <p> "HEY"->ReasonReact.string </p>}
      </BrowserWeb3Capabilities>
      <p>
        "isLoggedIn status"->ReasonReact.string
        <br />
        ("OCaml ftw " ++ self.state.isLoggedIn->string_of_bool)
        ->ReasonReact.string
      </p>
      <button onClick={_ => initUppy()} id="select-files">
        "CLICK ME"->ReasonReact.string
      </button>
      <button
        onClick={
          _ =>
            ThreeBox.openBox(
              ThreeBox.web3##eth##accounts[0],
              ThreeBox.web3##currentProvider,
            )
            |> Repromise.andThen(value => {
                 Js.log(value);
                 self.send(SetLoggedIn(true));
                 self.send(SetThreeBox(value));
                 Repromise.resolved(value);
               })
            |> Repromise.wait(Js.log)
        }>
        "LOGIN"->ReasonReact.string
      </button>
      <button
        onClick={
          _ =>
            Belt.Option.mapWithDefault(
              Js.Nullable.toOption(self.state.threeBox),
              _ => Js.log("NO THREE BOX"),
              threeBox => {
                Js.log(threeBox);
                ThreeBox.logout(threeBox);
              },
              (),
            )
        }>
        "LOGOUT"->ReasonReact.string
      </button>
      <GatsbyLink
        style={ReactDOMRe.Style.make(~margin="0", ())} to_="/page-2">
        {ReasonReact.string("GatsbyLink To Page 2")}
      </GatsbyLink>
    </div>,
};

let default =
  ReasonReact.wrapReasonForJs(~component, jsProps =>
    make(~data=jsProps##data, jsProps##children)
  );