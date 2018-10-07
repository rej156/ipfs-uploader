[@bs.module "./lib/init-uppy.js"] external initUppy: unit => unit = "default";

type state = {
  isLoggedIn: bool,
  threeBox: Js.Nullable.t(ThreeBox.threeBox),
};

type action =
  | SetLoggedIn(bool)
  | SetThreeBox(ThreeBox.threeBox);
let component = ReasonReact.reducerComponent("App");

let make = _children => {
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
            |> Js.Promise.then_(value => {
                 Js.log(value);
                 self.send(SetLoggedIn(true));
                 self.send(SetThreeBox(value));
                 Js.Promise.resolve(value);
               })
            |> ignore
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
                ThreeBox.logout(threeBox)
                |> Js.Promise.then_(_ => {
                     Js.log(threeBox);
                     self.send(SetLoggedIn(false));
                     Js.Promise.resolve();
                   })
                |> ignore;
                () => ();
              },
              (),
            )
        }>
        "LOGOUT"->ReasonReact.string
      </button>
    </div>,
};

let default =
  ReasonReact.wrapReasonForJs(~component, jsProps => make(jsProps##children));