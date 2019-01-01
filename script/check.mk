# This file is part of cerebrum.

# cerebrum is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# cerebrum is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with cerebrum.  If not, see <https://www.gnu.org/licenses/>.

define check
check: coding-style
coding-style:
	@RET=0; \
	for i in $(1); do \
		echo -e "\033[93m[check]:\033[95m" $$$$i "\033[0m"; \
		$(top_srcdir)/script/checkpatch.pl \
			--no-tree \
			--no-summary \
			--terse \
			--show-types \
			-f \
			--ignore SPLIT_STRING \
			--ignore LEADING_SPACE \
			--ignore OPEN_BRACE \
			$$$$i; \
		if [ $$$$? -ne 0 ]; then \
			let "RET=1"; \
		fi; \
	done; \
	if [ $$$$RET -ne 0 ]; then \
		echo -e "\033[93m[check]: \033[91mcheck failed\033[0m"; \
		exit $$$$RET; \
	else \
		echo -e "\033[93m[check]: \033[92mcheck succeed\033[0m"; \
		exit 0; \
	fi
endef
