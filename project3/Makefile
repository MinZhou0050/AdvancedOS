CC=gcc
LIB=-lpthread -Wall

aubatch: aubatch.c commandline.o schedule.o
	$(CC) -o aubatch aubatch.c commandline.o schedule.o $(-lpthread -Wall)
	@echo "Aubatch compile finished!"

commandline.o: commandline.c
	$(CC) -c commandline.c $(-lpthread -Wall)
	@echo "Commandline compile finished!"

schedule.o: schedule.c
	$(CC) -c schedule.c $(-lpthread -Wall)
	@echo "Schedule compile finished!"

clean:
	rm -f aubatch
	@echo "Removing finished!"
