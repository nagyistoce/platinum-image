Platinum is composed of four "manager" objects that collect and process data based on the model-view-controller (MVC) pattern.  This architecture had been selected prior to project onset – the MVC pattern was introduced subsequently to guide development, as well as to impart familiarity to developers who may get involved at a later stage. All object exchange between these managers is governed by [IDs](IDs.md).

## Datamanager ##
[datamanager](datamanager.md) manages the creation, deletion and retrieval of volume data sets – model.
## Viewmanager ##
[viewmanager](viewmanager.md) manages refresh and selection of viewports in which rendered images of volume data are displayed - controller, where each viewport object that is "managed" comprises a view.
## Rendermanager ##
The [rendermanager](rendermanager.md) connects renderer objects with viewports and image data.
## User IO manager ##
[userIOmanager](userIOmanager.md) lists and relays queries/commands between the user and applications implemented in the platform – editor.