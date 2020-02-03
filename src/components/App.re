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
  isLoggingIn: bool,
  isLoggedIn: bool,
  ipfsHash: string,
  files: array(file),
  threeBox: Js.Nullable.t(ThreeBox.threeBox),
};

type action =
  | SetLoggedIn(bool)
  | SetLoggingIn(bool)
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
    isLoggingIn: false,
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
    | SetLoggingIn(status) =>
      ReasonReact.Update({...state, isLoggingIn: status})
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
        <Dialog fullScreen=true open_={self.state.isLoggingIn}>
          <DialogContent
            style={
              ReactDOMRe.Style.make(
                ~height="100%",
                ~display="flex",
                ~flexDirection="column",
                ~justifyContent="center",
                ~alignItems="center",
                (),
              )
            }>
            <CircularProgress />
            <DialogContentText>
              <Typography gutterBottom=true variant=`Body1>
                "Logging into 3box!"
              </Typography>
            </DialogContentText>
          </DialogContent>
        </Dialog>
        <Header siteTitle=data##site##siteMetadata##title />
        {
          Js.Array.length(self.state.files) > 0 ?
            <Typography gutterBottom=true variant=`H5>
              "Your private list of files"->ReasonReact.string
            </Typography> :
            <Typography gutterBottom=true variant=`H5>
              "Upload some files!"->ReasonReact.string
            </Typography>
        }
        {
          Js.Array.length(self.state.files) > 0 ?
            <GridList cols={`Int(4)}>
              {
                self.state.files
                ->Belt.Array.mapWithIndex((index, file) =>
                    <Grid item=true key={file##hash ++ index->string_of_int}>
                      <div
                        style={ReactDOMRe.Style.make(~cursor="pointer", ())}
                        onClick={_ => self.send(RemoveFile(index))}>
                        <Avatar>
                          <Icon color=`Primary>
                            "delete"->ReasonReact.string
                          </Icon>
                        </Avatar>
                      </div>
                      <TextField
                        fullWidth=true
                        disabled=true
                        defaultValue={`String(file##hash)}
                      />
                      <TextField
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
                        value={`String(file##name)}
                      />
                      <Typography variant=`Title>
                        {
                          "Uploaded at: "
                          ++ (
                            file##date
                            |> Js.Date.fromString
                            |> Js.Date.getFullYear
                            |> string_of_float
                          )
                          ++ (
                            file##date
                            |> Js.Date.fromString
                            |> Js.Date.getMonth
                            |> string_of_float
                          )
                          ++ (
                            file##date
                            |> Js.Date.fromString
                            |> Js.Date.getDay
                            |> string_of_float
                          )
                          ++ " - "
                          ++ (
                            file##date
                            |> Js.Date.fromString
                            |> Js.Date.getHours
                            |> string_of_float
                          )
                          ++ (
                            file##date
                            |> Js.Date.fromString
                            |> Js.Date.getMinutes
                            |> string_of_float
                          )
                        }
                      </Typography>
                    </Grid>
                  )
                /*
                 yyyy-MM-ddThh:mm
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
                    </span> */
                |> ReasonReact.array
              }
            </GridList> :
            ReasonReact.null
        }
        {
          Js.Array.length(self.state.files) > 0 ?
            <Button
              color=`Primary
              variant=`Outlined
              onClick={_ => self.send(SaveFiles)}>
              "Save file data"->ReasonReact.string
            </Button> :
            ReasonReact.null
        }
        {
          if (Js.String.length(self.state.ipfsHash) > 0) {
            <Grid container=true direction=`Row spacing=V8>
              <Grid item=true>
                <Typography variant=`H6>
                  "Last uploaded file: "->ReasonReact.string
                </Typography>
              </Grid>
              <Grid item=true xs=V6>
                <TextField
                  fullWidth=true
                  disabled=true
                  defaultValue={`String(self.state.ipfsHash)}
                />
              </Grid>
            </Grid>;
          } else {
            ReasonReact.null;
          }
        }
        <Grid container=true spacing=V16>
          <Grid item=true>
            <Button
              onClick={
                _ => initUppy(ipfsHash => self.send(PersistFile(ipfsHash)))
              }
              color=`Primary
              variant=`Raised
              className="select-files">
              (
                self.state.isLoggedIn ?
                  "Upload a file to your 3box account" : "Upload a file"
              )
              ->ReasonReact.string
            </Button>
          </Grid>
          <Grid item=true>
            <BrowserWeb3Capabilities
              isLoggedIn={self.state.isLoggedIn} loggedInAddress="123456789">
              ...{
                   ({hasWeb3, isLockedAccount}) =>
                     hasWeb3 ?
                       !self.state.isLoggedIn ?
                         isLockedAccount ?
                           <Typography color=`Secondary variant=`H6>
                             "Unlock your account to login and save files!"
                             ->ReasonReact.string
                           </Typography> :
                           <Button
                             color=`Secondary
                             variant=`Outlined
                             onClick={
                               _ => {
                                 self.send(SetLoggingIn(true));
                                 ThreeBox.openBox(
                                   ThreeBox.web3##eth##accounts[0],
                                   ThreeBox.web3##currentProvider,
                                 )
                                 ->Promise.flatMap(value => {
                                      self.send(SetThreeBox(value));
                                      Promise.resolved(value);
                                    })
                                 ->Promise.get(box => {
                                      Js.log(box);
                                      self.send(SetLoggingIn(false));
                                    });
                               }
                             }>
                             "Login to save your files to your 3box account!"
                             ->ReasonReact.string
                           </Button> :
                         <Button
                           color=`Secondary
                           variant=`Outlined
                           onClick={_ => self.send(Logout)}>
                           "Logout"->ReasonReact.string
                         </Button> :
                       <Button
                         color=`Secondary
                         variant=`Outlined
                         href="https://metamask.io">
                         "Install MetaMask to save your files to a 3box account!"
                         ->ReasonReact.string
                       </Button>
                 }
            </BrowserWeb3Capabilities>
          </Grid>
        </Grid>
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