OBJS=umset.go extset.go certs.go eis.go error-handler.go fsm.go simple_db.go utils.go va.go unordered-set.go unordered-map.go star-map.go serializer.go web.go brg.go lazy-seq.go ethlib.go domain-fiber-server.go nn.go x11draw.go avl.go eqt.go opencv.go compress.go sha-1.go base64.go websocket.go onvif-client.go

OBJS=umset.go extset.go eis.go error-handler.go simple_db.go utils.go va.go unordered-set.go unordered-map.go star-map.go serializer.go web.go brg.go ethlib.go nn.go avl.go eqt.go opencv.go compress.go sha-1.go base64.go websocket.go onvif-client.go

.SUFFIXES: .scm .go

all: $(OBJS)
	echo Compilation terminated

.scm.go:
	guild compile -O3 $< -o $@
	sudo cp $@ /new_devs/usr/local/mtfa-guile-libs/mtfa/

compress.go: utils.go error-handler.go serializer.go simple_db.go eis.go unordered-set.go unordered-map.go star-map.go extset.go umset.go
brg.go: utils.go error-handler.go serializer.go simple_db.go eis.go unordered-set.go unordered-map.go star-map.go extset.go umset.go
web.go: web.scm utils.go error-handler.go serializer.go simple_db.go eis.go unordered-set.go unordered-map.go star-map.go extset.go umset.go
certs.go: certs.scm utils.go simple_db.go
eis.go: eis.scm utils.go
error-handler.go: error-handler.scm
utils.go: utils.scm error-handler.go
fsm.go: fsm.scm utils.go serializer.go
simple_db.go: simple_db.scm utils.go
va.go: va.scm utils.go
unordered-set.go: unordered-set.scm utils.go serializer.go
unordered-map.go: unordered-map.scm utils.go serializer.go
star-map.go: star-map.scm utils.go
extset.go: extset.scm utils.go
umset.go: umset.scm extset.go utils.go serializer.go
serializer.go: serializer.scm utils.go
lazy-seq.go: lazy-seq.scm utils.go
ethlib.go: ethlib.scm utils.go
domain-fiber-server.go: domain-fiber-server.scm utils.go
nn.go: nn.scm utils.go opencv.go
x11draw.go: x11draw.scm
avl.go: avl.scm utils.go
eqt.go: eqt.scm utils.go serializer.go
opencv.go: opencv.scm utils.go serializer.go umset.go
sha-1.go: sha-1.scm
base64.go: base64.scm
websocket.go: websocket.scm sha-1.go base64.go error-handler.go
onvif-client.go: onvif-client.scm

clean:
	/bin/rm -f $(OBJS)
