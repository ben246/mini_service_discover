GOPATH := $(shell pwd):$(GOPATH)


OBJS := client \
        daemon \
        server

define build_all
@for name in $(1); \
	do \
		echo "go build $${name}.go"; \
		go build $${name}.go; \
	done
endef

all:
	$(call build_all, $(OBJS))
		
.PHONY : all clean
  
clean :
	@rm -f $(OBJS)

