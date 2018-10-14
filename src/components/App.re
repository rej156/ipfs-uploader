[@bs.module "../lib/init-uppy.js"]
external initUppy: (string => unit) => unit = "default";

[@bs.module "../lib/Store.js"]
external storeFile: (ThreeBox.threeBox, string) => unit = "storeFile";

[@bs.module "../lib/Store.js"]
external fetchFiles: (ThreeBox.threeBox, array(string) => unit) => unit =
  "fetchFiles";

type state = {
  isLoggedIn: bool,
  ipfsHash: string,
  files: array(string),
  threeBox: Js.Nullable.t(ThreeBox.threeBox),
};

type action =
  | SetLoggedIn(bool)
  | Logout
  | PersistFile(string)
  | SetFiles(array(string))
  | FetchFiles
  | SetThreeBox(ThreeBox.threeBox);
let component = ReasonReact.reducerComponent("App");

type data = {. "site": {. "siteMetadata": {. "title": string}}};

let meta = [|
  Helmet.metaField(~name="description", ~content="Sample"),
  Helmet.metaField(~name="keywords", ~content="sample, something"),
|];

let extractBox = (threeBox, callback) =>
  Belt.Option.mapWithDefault(
    Js.Nullable.toOption(threeBox),
    (),
    box => {
      Js.log(box);
      callback(box);
    },
  );

let make = (~data, _children) => {
  ...component,
  initialState: () => {
    isLoggedIn: false,
    threeBox: Js.Nullable.undefined,
    ipfsHash: "",
    files: [||],
  },
  reducer: (action, state) =>
    switch (action) {
    | SetLoggedIn(status) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, isLoggedIn: status},
        (self => self.send(FetchFiles)),
      )
    | SetFiles(files) => ReasonReact.Update({...state, files})
    | FetchFiles =>
      ReasonReact.SideEffects(
        (
          self =>
            extractBox(self.state.threeBox, box =>
              fetchFiles(box, files => self.send(SetFiles(files)))
            )
        ),
      )
    | SetThreeBox(threeBox) =>
      ReasonReact.Update({...state, threeBox: Js.Nullable.return(threeBox)})
    | PersistFile(ipfsHash) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, ipfsHash},
        (
          self => {
            extractBox(self.state.threeBox, box => storeFile(box, ipfsHash));
            Js.Global.setTimeout(() => self.send(FetchFiles), 500) |> ignore;
          }
        ),
      )
    | Logout =>
      ReasonReact.SideEffects(
        (
          self =>
            extractBox(
              self.state.threeBox,
              box => {
                self.send(SetLoggedIn(false));
                ThreeBox.logout(box);
              },
            )
        ),
      )
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
      /* <BrowserWeb3Capabilities
           isLoggedIn={self.state.isLoggedIn} loggedInAddress="123456789">
           ...{_ => <p> "HEY"->ReasonReact.string </p>}
         </BrowserWeb3Capabilities> */
      {
        self.state.files
        ->Belt.Array.map(file => <p key=file> file->ReasonReact.string </p>)
        |> ReasonReact.array
      }
      <p>
        ("isLoggedIn status: " ++ self.state.isLoggedIn->string_of_bool)
        ->ReasonReact.string
      </p>
      {
        if (Js.String.length(self.state.ipfsHash) > 0) {
          <p>
            ("Last uploaded file: " ++ self.state.ipfsHash)->ReasonReact.string
          </p>;
        } else {
          ReasonReact.null;
        }
      }
      <button
        onClick={_ => initUppy(ipfsHash => self.send(PersistFile(ipfsHash)))}
        id="select-files">
        "Upload a file"->ReasonReact.string
      </button>
      <button
        onClick={
          _ =>
            ThreeBox.openBox(
              ThreeBox.web3##eth##accounts[0],
              ThreeBox.web3##currentProvider,
            )
            |> Repromise.andThen(value => {
                 self.send(SetThreeBox(value));
                 self.send(SetLoggedIn(true));
                 Repromise.resolved(value);
               })
            |> Repromise.wait(Js.log)
        }>
        "LOGIN"->ReasonReact.string
      </button>
      <button onClick={_ => self.send(Logout)}>
        "LOGOUT"->ReasonReact.string
      </button>
    </div>,
  /* <GatsbyLink
       style={ReactDOMRe.Style.make(~margin="0", ())} to_="/page-2">
       {ReasonReact.string("GatsbyLink To Page 2")}
     </GatsbyLink> */
};

let default =
  ReasonReact.wrapReasonForJs(~component, jsProps =>
    make(~data=jsProps##data, jsProps##children)
  );