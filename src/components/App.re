type file = {
  .
  "name": string,
  "hash": string,
  "date": string,
};

[@bs.module "../lib/init-uppy.js"]
external initUppy: (string => unit) => unit = "default";

[@bs.module "../lib/Store.js"]
external storeFile: (ThreeBox.threeBox, string) => unit = "storeFile";

[@bs.module "../lib/Store.js"]
external saveFiles: (ThreeBox.threeBox, array(file), string => unit) => unit =
  "saveFiles";

[@bs.module "../lib/Store.js"]
external fetchFiles: (ThreeBox.threeBox, array(file) => unit) => unit =
  "fetchFiles";

type state = {
  isLoggedIn: bool,
  ipfsHash: string,
  files: array(file),
  threeBox: Js.Nullable.t(ThreeBox.threeBox),
};

type action =
  | SetLoggedIn(bool)
  | Logout
  | PersistFile(string)
  | SetFiles(array(file))
  | SetFilename((int, string))
  | FetchFiles
  | SaveFiles
  | RemoveFile(int)
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
    | SetFilename((index, name)) =>
      let newFile = state.files[index];
      let newFile = Js.Obj.assign(newFile, {"name": name});
      let newFiles = state.files;
      Belt.Array.setUnsafe(newFiles, index, newFile);
      ReasonReact.Update({...state, files: newFiles});
    | RemoveFile(index) =>
      let newFiles =
        Belt.Array.keepWithIndex(state.files, (_file, fileIndex) =>
          fileIndex !== index
        );
      ReasonReact.UpdateWithSideEffects(
        {...state, files: newFiles},
        (self => self.send(SaveFiles)),
      );
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
      ReasonReact.UpdateWithSideEffects(
        {...state, threeBox: Js.Nullable.return(threeBox)},
        (self => self.send(SetLoggedIn(true))),
      )
    | SaveFiles =>
      ReasonReact.SideEffects(
        (
          self =>
            extractBox(self.state.threeBox, box =>
              saveFiles(box, self.state.files, files => Js.log(files))
            )
        ),
      )

    | PersistFile(ipfsHash) =>
      ReasonReact.UpdateWithSideEffects(
        {...state, ipfsHash},
        (
          self => {
            extractBox(self.state.threeBox, box => storeFile(box, ipfsHash));
            Js.Global.setTimeout(() => self.send(FetchFiles), 800) |> ignore;
          }
        ),
      )
    | Logout =>
      ReasonReact.UpdateWithSideEffects(
        {...state, isLoggedIn: false, files: [||]},
        (
          self => extractBox(self.state.threeBox, box => ThreeBox.logout(box))
        ),
      )
    },
  render: self =>
    MaterialUi.(
      <div>
        <Helmet title=data##site##siteMetadata##title meta>
          <link
            href="https://transloadit.edgly.net/releases/uppy/v0.27.5/dist/uppy.min.css"
            rel="stylesheet"
          />
        </Helmet>
        <Header siteTitle=data##site##siteMetadata##title />
        {
          Js.Array.length(self.state.files) > 0 ?
            <h3> "Your private list of files"->ReasonReact.string </h3> :
            <h3> "Upload some files!"->ReasonReact.string </h3>
        }
        {
          Js.Array.length(self.state.files) > 0 ?
            <List>
              {
                self.state.files
                ->Belt.Array.mapWithIndex((index, file) =>
                    <ListItem key={file##hash ++ index->string_of_int}>
                      <button onClick={_ => self.send(RemoveFile(index))}>
                        <Avatar> {j|❎|j}->ReasonReact.string </Avatar>
                      </button>
                      <input ariaReadonly=true defaultValue=file##hash />
                      <input
                        onChange={
                          event =>
                            self.send(
                              SetFilename((
                                index,
                                ReactEvent.Form.target(event)##value,
                              )),
                            )
                        }
                        placeholder="File name"
                        value=file##name
                      />
                      <span> "Uploaded at: "->ReasonReact.string </span>
                      <span>
                        {
                          (
                            file##date
                            |> Js.Date.fromString
                            |> Js.Date.getUTCFullYear
                            |> string_of_float
                          )
                          ++ (
                            file##date
                            |> Js.Date.fromString
                            |> Js.Date.getMonth
                            |> (month => month +. 1.0)
                            |> string_of_float
                          )
                          ++ (
                            file##date
                            |> Js.Date.fromString
                            |> Js.Date.getDate
                            |> string_of_float
                          )
                          |> ReasonReact.string
                        }
                      </span>
                    </ListItem>
                  )
                |> ReasonReact.array
              }
              <button onClick={_ => self.send(SaveFiles)}>
                "Save file data"->ReasonReact.string
              </button>
            </List> :
            ReasonReact.null
        }
        {
          if (Js.String.length(self.state.ipfsHash) > 0) {
            <div>
              <p> "Last uploaded file: "->ReasonReact.string </p>
              <input defaultValue={self.state.ipfsHash} ariaReadonly=true />
            </div>;
          } else {
            ReasonReact.null;
          }
        }
        <button
          onClick={
            _ => initUppy(ipfsHash => self.send(PersistFile(ipfsHash)))
          }
          id="select-files">
          (
            self.state.isLoggedIn ?
              "Upload a file to your 3box account" : "Upload a file"
          )
          ->ReasonReact.string
        </button>
        <BrowserWeb3Capabilities
          isLoggedIn={self.state.isLoggedIn} loggedInAddress="123456789">
          ...{
               ({hasWeb3}) =>
                 hasWeb3 ?
                   !self.state.isLoggedIn ?
                     <button
                       onClick={
                         _ =>
                           ThreeBox.openBox(
                             ThreeBox.web3##eth##accounts[0],
                             ThreeBox.web3##currentProvider,
                           )
                           |> Repromise.andThen(value => {
                                self.send(SetThreeBox(value));
                                Repromise.resolved(value);
                              })
                           |> Repromise.wait(Js.log)
                       }>
                       "Login to save your files to your 3box account!"
                       ->ReasonReact.string
                     </button> :
                     <button onClick={_ => self.send(Logout)}>
                       "Logout"->ReasonReact.string
                     </button> :
                   <a href="https://metamask.io">
                     "Install MetaMask to save your files to a 3box account!"
                     ->ReasonReact.string
                   </a>
             }
        </BrowserWeb3Capabilities>
      </div>
    ),
  /* <GatsbyLink
       style={ReactDOMRe.Style.make(~margin="0", ())} to_="/page-2">
       {ReasonReact.string("GatsbyLink To Page 2")}
     </GatsbyLink> */
};

let default =
  ReasonReact.wrapReasonForJs(~component, jsProps =>
    make(~data=jsProps##data, jsProps##children)
  );