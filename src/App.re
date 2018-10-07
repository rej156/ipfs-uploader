[@bs.module "./lib/init-uppy.js"]
external initUppy: (string => unit) => unit = "default";

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
  /* didMount: self =>
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
     |> ignore, */
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
        ...{
             ({hasWeb3}) =>
               self.state.isLoggedIn ?
                 <p> "You are now logged in"->ReasonReact.string </p> :
                 hasWeb3 ?
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
                     "LOGIN TO SAVE YOUR FILES"->ReasonReact.string
                   </button> :
                   <p>
                     "No web3 to login and save! Please install Metamask!"
                     ->ReasonReact.string
                   </p>
           }
      </BrowserWeb3Capabilities>
      <button
        onClick={
          _ =>
            initUppy(resultFileHash =>
              Belt.Option.mapWithDefault(
                Js.Nullable.toOption(self.state.threeBox),
                Js.log("NO THREE BOX"),
                threeBox => {
                  Js.log(resultFileHash);
                  let privateStoreSet = threeBox##private##set;

                  privateStoreSet("ipfsUploader.savedFile", resultFileHash)
                  |> Js.Promise.then_(_ => {
                       Js.log(resultFileHash ++ " saved");
                       Js.Promise.resolve(true);
                     })
                  |> ignore;
                },
              )
            )
        }
        id="select-files">
        "Upload file"->ReasonReact.string
      </button>
      {
        self.state.isLoggedIn ?
          <button
            onClick={
              _ =>
                Belt.Option.mapWithDefault(
                  Js.Nullable.toOption(self.state.threeBox),
                  _ => Js.log("NO THREE BOX"),
                  threeBox => {
                    let logout = threeBox##logout;

                    logout()
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
          </button> :
          ReasonReact.null
      }
    </div>,
};

let default =
  ReasonReact.wrapReasonForJs(~component, jsProps => make(jsProps##children));